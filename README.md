
# TP Synthèse – client TFTP
Ces lignes de commandes permettent de récupérer un fichiers texte sur le serveur Ensea "srvtpinfo1.ensea.fr" et cet la commande "cat Worldo,txt" on lit le contenu de ce fichier texte.
![fichier serveur ensea](https://github.com/Abdel126/TP_Networks-/assets/131591766/6cc92547-e2df-49d5-a39f-5cf71e436220)

Une autre manipulation mais cett fois on se connecter sur serveur local dans la commande utilisée : 
"-l" -> met le port 1069 en écoute 
"-u" -> commincation en mode UDP "sans acquittement de de réception"
"hexdump -c" affichage de la requette en lettre alphabétique
"hexdump -C" affichage de la requette en hexadécimal
Pour se connecter au  serveur il faut d'abord  mettre le port 1069 en écoute et ensuite établir la connexion avec le serveur.

![VirtualBox_StudentLab_20_12_2023_20_07_16](https://github.com/Abdel126/TP_Networks-/assets/131591766/8b6b5d1f-9498-481f-a244-49e484691c44)

On peut donc faire la correspondance en les octets qui composent la requette et le tableau ci dessous. par exemple les premier octets "00 01" correspondent a l'opération Read request(RRQ).

<img width="646" alt="Capture d'écran 2023-12-20 200823" src="https://github.com/Abdel126/TP_Networks-/assets/131591766/99a380f0-2e0b-49ac-9fc3-eee9bb45a6b4">

























