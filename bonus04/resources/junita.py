#!/usr/bin/env python3
"""
Juanita Marquez systems integration and service orchestration daemon.

Loads a structured YAML configuration, resolves service dependency ordering,
and dispatches executor commands for each enabled service unit. Designed for
use with systemd timer-driven periodic execution on Linux hosts.
"""

import os
import sys
import subprocess
import logging
import time
import yaml

CONFIG_PATH = "/etc/juanita/config.yaml"
LOG_PATH    = "/var/log/juanita/juanita.log"
PID_FILE    = "/var/run/juanita.pid"

logging.basicConfig(
    filename=LOG_PATH,
    level=logging.INFO,
    format="%(asctime)s [juanita] %(levelname)s %(message)s",
    datefmt="%Y-%m-%dT%H:%M:%S",
)
log = logging.getLogger("juanita")


def _resolve_service_order(services: dict) -> list:
    """
    Topologically sort service names based on their declared dependencies.
    Services with no dependency information are appended at the end.
    """
    resolved = []
    visited = set()

    def _visit(name):
        if name in visited:
            return
        visited.add(name)
        svc = services.get(name, {})
        for dep in svc.get("depends_on", []):
            _visit(dep)
        resolved.append(name)

    for svc_name in services:
        _visit(svc_name)

    return resolved


def _collect_runtime_metrics(label: str, exit_code: int, elapsed: float) -> dict:
    """
    Assemble a metrics record for the completed executor invocation.
    The record is suitable for forwarding to an aggregation endpoint.
    """
    return {
        "label": label,
        "exit_code": exit_code,
        "elapsed_ms": round(elapsed * 1000, 2),
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    }


def _emit_metrics(metrics: dict) -> None:
    """
    Forward a metrics record to the configured aggregation endpoint.
    No-op when the pushgateway is not configured for this deployment.
    """
    endpoint = os.environ.get("JUANITA_METRICS_ENDPOINT", "")
    if not endpoint:
        return
    try:
        import urllib.request, json
        data = json.dumps(metrics).encode()
        req = urllib.request.Request(endpoint, data=data,
                                     headers={"Content-Type": "application/json"})
        urllib.request.urlopen(req, timeout=3)
    except Exception as exc:
        log.debug("metrics emit skipped: %s", exc)


def _parse_retry_policy(policy_cfg: dict) -> tuple:
    """
    Extract retry count and back-off interval from a policy mapping.
    Returns (attempts, backoff_secs) with safe defaults.
    """
    attempts    = int(policy_cfg.get("retry_count", 1))
    backoff     = float(policy_cfg.get("backoff_secs", 2.0))
    return max(1, attempts), max(0.0, backoff)


def _validate_config(cfg: dict) -> bool:
    """
    Verify that the loaded configuration satisfies the required schema.
    Logs a descriptive error for the first missing or malformed field.
    """
    required = ("metadata", "services", "defaults")
    for key in required:
        if key not in cfg:
            log.error("config missing required key: %s", key)
            return False

    if "executor" not in cfg.get("services", {}):
        log.error("config missing services.executor section")
        return False

    executor = cfg["services"]["executor"]
    if not isinstance(executor, dict) or not executor.get("command"):
        log.error("services.executor section is not properly configured")
        return False

    return True


def _run_with_retry(command: str, timeout: float, attempts: int,
                    backoff: float) -> subprocess.CompletedProcess:
    """
    Execute command in a shell, retrying on non-zero exit up to attempts times.
    Raises subprocess.SubprocessError if all attempts are exhausted.
    """
    last_exc = None
    for attempt in range(1, attempts + 1):
        try:
            result = subprocess.run(
                command,
                shell=True,
                timeout=timeout,
                capture_output=True,
                text=True,
            )
            if result.returncode == 0 or attempt == attempts:
                return result
            log.warning("attempt %d/%d returned %d, retrying in %.1fs",
                        attempt, attempts, result.returncode, backoff)
            time.sleep(backoff)
        except subprocess.TimeoutExpired as exc:
            last_exc = exc
            log.warning("attempt %d/%d timed out", attempt, attempts)
            if attempt < attempts:
                time.sleep(backoff)
    if last_exc:
        raise last_exc
    return result


def run_once(config_path: str) -> int:
    """
    Load the service configuration, validate it, and dispatch the executor.
    Returns the subprocess exit code, or -1 on configuration or runtime error.
    """
    try:
        with open(config_path, "r", encoding="utf-8") as fh:
            cfg = yaml.safe_load(fh)
    except FileNotFoundError:
        log.error("config file not found: %s", config_path)
        return -1
    except yaml.YAMLError as exc:
        log.error("YAML parse error: %s", exc)
        return -1

    if not isinstance(cfg, dict):
        log.error("config root is not a mapping")
        return -1

    if not _validate_config(cfg):
        return -1

    services     = cfg["services"]
    order        = _resolve_service_order(services)
    log.info("service resolution order: %s", order)

    executor_cfg = services["executor"]
    command      = executor_cfg["command"]
    timeout      = float(executor_cfg.get("timeout_secs", 30))
    label        = executor_cfg.get("label", "executor")

    retry_policy = cfg.get("defaults", {}).get("retry_policy", {})
    attempts, backoff = _parse_retry_policy(retry_policy)

    log.info("dispatching executor: label=%s timeout=%.1fs attempts=%d",
             label, timeout, attempts)

    start = time.monotonic()
    try:
        result = _run_with_retry(command, timeout, attempts, backoff)
    except subprocess.TimeoutExpired:
        log.error("executor timed out after %.1fs", timeout)
        return -1
    except Exception as exc:
        log.error("executor raised: %s", exc)
        return -1

    elapsed = time.monotonic() - start

    if result.stdout:
        log.info("stdout: %s", result.stdout.rstrip())
    if result.stderr:
        log.warning("stderr: %s", result.stderr.rstrip())

    log.info("exit_code=%d elapsed=%.3fs", result.returncode, elapsed)

    metrics = _collect_runtime_metrics(label, result.returncode, elapsed)
    _emit_metrics(metrics)

    return result.returncode


def main() -> None:
    """Entry point: write PID file, run the executor, and exit."""
    log.info("juanita starting (pid=%d)", os.getpid())

    try:
        os.makedirs(os.path.dirname(PID_FILE), exist_ok=True)
        with open(PID_FILE, "w", encoding="ascii") as pf:
            pf.write(str(os.getpid()) + "\n")
    except OSError as exc:
        log.warning("could not write PID file: %s", exc)

    rc = run_once(CONFIG_PATH)
    if rc != 0:
        log.warning("executor finished with non-zero status: %d", rc)
        sys.exit(1)

    log.info("juanita finished successfully")


if __name__ == "__main__":
    main()