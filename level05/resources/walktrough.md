There is two files in /opt/snowcrash/level05 :

metaverse.wasm and metaverse.wat.

`metaverse.wat` is a web assembly file and `metaverse.wasm` is the executable version of the .wat file.

In parallel to that there is also like in the previous exercices a service launches by a timer.

```
cat /etc/systemd/system/metaverse.timer 

[Unit]
Description=Metaverse WASM runtime — runs every 30 seconds
Requires=metaverse.service

[Timer]
OnBootSec=15s
OnUnitActiveSec=30s
AccuracySec=1s

[Install]
WantedBy=timers.target

```

```
cat /etc/systemd/system/metaverse.service 

[Unit]
Description=Metaverse WASM runtime (one-shot)
After=network.target

[Service]
Type=oneshot
User=flag05
ExecStart=/usr/local/bin/wasmtime --dir=/ /opt/snowcrash/level05/metaverse.wasm
StandardOutput=journal
StandardError=journal
```

The service launches `metaverse.wasm` with flag05 rights. We have the right to modify this file so by recompiling a .wat file into `metaverse.wasm` we can make it do whatever we want.

The `new.wat` in `ressources/` allows us to retrieves the flag.
It read the .flag file in the home folder of flag05 and writes it back in `/tmp/metaverse_flag`.

So by compiling this file to replace `metaverse.wasm` like this `wat2wasm /opt/snowcrash/level05/new.wat -o /opt/snowcrash/level05/test.wasm`.

By doing that the service will executes it and write the flag in the temp file.
