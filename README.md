# Server_chat
TD13 & TD14 INSA
https://github.com/Sosso8305/Server_chat

list des commande:

/list -> pour obtenir la list des utilisateurs

/date -> pour obtenir la date via "fr.pool.ntp.org"

/nick <name> -> pour changer de prénom 

/me <name> <msg> -> envoyer un message privé

/file <name> <fileSRC> -> envoie une notification à <name> qu'on veut lui envoyer un fichier 

/decline <name> -> refuser l'envoie d'un fichier venant de <name>

/accept <name> <fileDEST> -> accepte l'envoie d'un fichier et donne son futur emplacement

L'envoie de fichier ok mais tu coté server la taille est limité (comme un vrai serveur pour eviter les transaaction de trop grosse donné cf Discord);

absorbe les mauvaise commande.

@all fonctionnais mais des fois pas accepter car j'ai des problemes,
Donc on affiche en plus *Beep* en rouge


existe une variable DEBUG