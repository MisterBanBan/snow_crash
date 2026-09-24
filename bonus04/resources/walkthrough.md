A service juanita launches every 60 seconds the python script located in /opt/snowcrash/bonus04/juanita.py.

When reding thius script we can see that it takes a configuration file located in /etc/juanita/config.yaml.

The script uses this config to do several things including executing a the command in services->executor->command.
Because we can write this file than to the juanita_grp group we can simpli chnage this command to /bin/cat /home/flagbonus04/.flag > /tmp/flagbonus04.

Doing that write the flag in the tmp/flagbonus04 file in the next script execution.