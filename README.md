# Snow Crash — 42 (Post Common Core)

Solutions du projet Snow Crash. Un dossier par niveau :

```
levelNN/
  flag          # le flag récupéré (= mot de passe SSH du niveau suivant)
  resources/    # démarche, commandes et scripts utilisés
```

- **Obligatoire** : `level00` → `level10` (level10 = boss).
- **Bonus** : `bonus01` → `bonus05` (évalué seulement si l'obligatoire est parfait).

## Environnement
VM lancée via QEMU (émulation x86 sur Mac Apple Silicon — VirtualBox impossible sur ARM).
Script de lancement dans le dossier parent : `../launch.sh`.
Connexion : `ssh levelNN@localhost -p 4242`.

## Règles de rendu
- **Aucun binaire** dans le repo (voir `.gitignore`) — les fichiers de la VM se téléchargent uniquement pour l'analyse / la défense.
- Chaque étape doit pouvoir être expliquée à l'oral.

## Avancement
- [x] level00
- [ ] level01 · level02 · level03 · level04 · level05 · level06 · level07 · level08 · level09 · level10
- [ ] bonus01 · bonus02 · bonus03 · bonus04 · bonus05
