TRTP Project
============

Ce projet a pour but d'envoyer des paquets en UDP et IpV6 par le biais du protocole suivant
![Protocole](Latex/imgs/Paquet.png)


Utilisation
===========

Makefile
---------

Pour compiler l'executable *receiver*, il suffit de faire :
```bash
make
```

Pour nettoyer tous les executables :
```bash
make clean
```

receiver
-----------

L'executable prend les arguments suivant :

```bash
./receiver [-o "Fichier %d.txt"] [-m 10] [-w] [-t 3600] [-l] :: 4455
```

- <span style="color:orange">-o</span>
<span style="color:#98D36C">"Fichier %d.txt"</span> : l'option prend un fichier de sortie avec un entier en argument pour créer un fichier par client. Par défault : <span style="color:#98D36C">"Fichier %d.txt"</span>.

- <span style="color:orange">-m</span> 10 : l'option prend un nombre maximum de conneections concurrentes en argument.

- <span style="color:orange">-w</span> : l'option active la fenêtre de réception variable. Celle-ci est adaptée en fonctions des erreurs de transmission.

- <span style="color:orange">-t</span> 3600 : l'option définit le temps d'attente du serveur. Lorsque qu'aucun client n'envoie de paquets durant cette période, le programme fini. Par défault : 30.

- <span style="color:orange">-l</span> : l'option permet d'afficher les *logs* sur la sortie standard.
