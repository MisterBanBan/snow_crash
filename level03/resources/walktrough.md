The CMakeLists.txt in /opt/snowcrash/level03/ opens a file that only flag03 can access thus it stops when executing with the level03 user.

We can also see that a timer is launched with this command: `cat /etc/systemd/system/burbclave.timer`

Showing a timer that launches this service `cat /etc/systemd/system/burbclave.service`

that launches the cmake with the flag03 user that have the permission to open the config file.

But in that state the execution of the service fails. ("systemctl status burbclave.service --no-pager")

We can see at the end of the Cmake that it includes and so execute this file : `include(${RESOLVED_BUILD_PATH}/build.cmake)`

We can't see what is the real path of this file but we can see that the user Level03 is part of the group : burbclave.

This groups owns only one folder : `find / -type d -group burbclave 2>/dev/null
/var/lib/burbclave
/var/lib/burbclave/staging`

We can deduct that the `RESOLVED_BUILD_PATH` is thsi folder `/var/lib/burbclave/staging`.

So but putting a build.cmake in this folder it will be launched by the service with flag03 user. 

So by putting a build.cmake that reads the .flag and copy it's content into a file accessible by level03 we can retreive the flag.

A corresponding build.cmake id present in ressources/ it contains :

`execute_process(COMMAND  sh -c "cat /home/flag03/.flag > /tmp/flag_out.txt; chmod 644 /tmp/flag_out.txt")`
