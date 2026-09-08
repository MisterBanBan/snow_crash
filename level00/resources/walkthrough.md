# level00 — Snow Crash

**But :** lire `/home/flag00/.flag`, illisible par `level00`, pour obtenir le mot de passe SSH de `level01`.

## Énumération

```
level00@snowcrash:~$ find / -name "level*" 2>/dev/null

/usr/share/X11/xkb/compat/level5
/usr/share/X11/xkb/types/level5
/usr/share/X11/xkb/symbols/level3
/usr/share/X11/xkb/symbols/level2
/usr/share/X11/xkb/symbols/level5
/sys/devices/system/cpu/cpu1/cache/index2/level
...
/home/level00
/home/level01
...
/opt/snowcrash/level00
/opt/snowcrash/level01
...
```

Cela nous permet de trouver toutes les routes contenant le mot « level » et de mettre les erreurs à la poubelle grâce à `2>/dev/null`. Ensuite nous allons dans `/opt/snowcrash/level00` :

```
level00@snowcrash:~$ cd /opt/snowcrash/level00
level00@snowcrash:/opt/snowcrash/level00$ ls
hiro
```

Cela nous donne un programme nommé `hiro` que nous allons exécuter pour voir ce qu'il fait.

## Exécution de hiro

```
level00@snowcrash:/opt/snowcrash/level00$ ./hiro
[*] Checking credentials...
[*] Access granted — welcome, flag00
flag00@snowcrash:~$ ls
total 12
drwxr-x--- 2 flag00 flag00 4096 Jan  1 00:00 .
drwxr-xr-x 22 root   root   4096 Jan  1 00:00 ..
-r-------- 1 flag00 flag00   32 Jan  1 00:00 .flag
-rwsr-x--- 1 flag00 level00  12288 Jan  1 00:00 hiro
flag00@snowcrash:~$ cat .flag
cat: permission denied
```

Au passage, `hiro` est **setuid** (`-rwsr-x---`) et appartient à **flag00** : lancé par `level00`, il s'exécute avec l'euid de flag00. Pourtant `cat .flag` échoue quand même.

En testant, nous voyons qu'il s'agit peut-être d'un shell. Pour cela nous allons faire un `strings` sur le programme pour extraire le texte visible :

```
drwxr-x--- 2 flag00 flag00 4096 Jan  1 00:00 .
drwxr-xr-x 22 root   root   4096 Jan  1 00:00 ..
-r-------- 1 flag00 flag00   32 Jan  1 00:00 .flag
-rwsr-x--- 1 flag00 level00  12288 Jan  1 00:00 hiro
cat: permission denied
sudo
```

Nous tombons sur du texte qui est écrit en dur et qui correspond à ce que nous avons tapé : c'est fait pour nous induire en erreur. Si nous tapons `ls`, nous obtenons un texte prédéfini.

Preuve que ce shell est simulé : `uname -a` renvoie un faux système Debian, alors que la vraie VM est en Ubuntu 24.04 (kernel 6.8). Le programme ment, donc toutes ses sorties sont fausses :

```
flag00@snowcrash:~$ uname -a
Linux SnowCrash 3.2.0-4-amd64 #1 SMP Debian 3.2.51-1 x86_64 GNU/Linux
```

Ensuite nous allons juste prendre les mots-clés qui nous intéressent, dont le mot `flag` :

```
level00@snowcrash:/opt/snowcrash/level00$ strings ./hiro | grep flag
flag00
flag00@snowcrash:~$ 
 welcome, flag00
uid=3000(flag00) gid=3000(flag00) groups=3000(flag00)
flag00
/home/flag00
HOME=/home/flag00
USER=flag00
LOGNAME=flag00
drwxr-x--- 2 flag00 flag00 4096 Jan  1 00:00 .
-r-------- 1 flag00 flag00   32 Jan  1 00:00 .flag
-rwsr-x--- 1 flag00 level00  12288 Jan  1 00:00 hiro
flag_data
g_shutdown_flag
.flag_data
```

Nous trouvons `flag_data`, donc nous regardons le contenu de cette section dans le programme `hiro` :

```
level00@snowcrash:/opt/snowcrash/level00$ objdump -d -j .flag_data hiro

hiro:     file format elf64-x86-64


Disassembly of section .flag_data:

0000000000004640 <flag_data>:
    4640:	66 32 61 76 35 69 6c 30 32 70 75 61 6e 6f 37 6e     f2av5il02puano7n
    4650:	61 61 66 36 61 64 61 66 33 61 00                    aaf6adaf3a.
```

Donc `objdump -d -j .flag_data hiro` : `-d` pour désassembler (lire les octets comme des instructions), `-j` pour cibler une seule section (`.flag_data`), et `hiro` pour dire que c'est dans ce programme que l'on cherche. Comme `.flag_data` contient de la donnée et non du code, objdump se contente d'afficher les octets bruts et leur ASCII à droite, ce qui suffit à lire la chaîne.

Nous trouvons donc le flag :

```
f2av5il02puano7naaf6adaf3a
```

Ce flag est le mot de passe SSH de `level01` :

```
$ ssh level01@localhost -p 4242
level01@snowcrash:~$
```

Ensuite nous passons au niveau 1.