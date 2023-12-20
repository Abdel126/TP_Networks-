
# TP Synthèse – client TFTP

Ces lignes de commandes permettent de récupérer un fichiers texte sur le serveur Ensea "srvtpinfo1.ensea.fr" et cet la commande "cat Worldo,txt" on lit le contenu de ce fichier texte.
![fichier serveur ensea](https://github.com/Abdel126/TP_Networks-/assets/131591766/6cc92547-e2df-49d5-a39f-5cf71e436220)

Une autre manipulation mais cett fois on se connecter sur serveur local dans la commande utilisée : 
"-l" -> met le port 1069 en écoute 
"-u" -> commincation en mode UDP "sans acquittement de de réception"
"hexdump -c" affichage de la requête  en lettre alphabétique
"hexdump -C" affichage de la requête  en hexadécimal
Pour se connecter au  serveur il faut d'abord  mettre le port 1069 en écoute et ensuite établir la connexion avec le serveur.

![VirtualBox_StudentLab_20_12_2023_20_07_16](https://github.com/Abdel126/TP_Networks-/assets/131591766/8b6b5d1f-9498-481f-a244-49e484691c44)

On peut donc faire la correspondance en les octets qui composent la requête  et le tableau ci dessous. par exemple les premier octets "00 01" correspondent a l'opération Read request(RRQ).

<img width="646" alt="Capture d'écran 2023-12-20 200823" src="https://github.com/Abdel126/TP_Networks-/assets/131591766/99a380f0-2e0b-49ac-9fc3-eee9bb45a6b4">


## Travail à Réaliser :

current = server_info; : Initialise le pointeur current pour qu'il pointe sur le premier élément de la liste server_info.
current != NULL; : Condition de poursuite de la boucle tant que current pointe vers une adresse valide (non NULL).
current = current->ai_next : Avance à l'élément suivant de la liste pour la prochaine itération de la boucle.
À chaque itération de la boucle, le code tente de créer un socket en utilisant socket(). Si la création du socket échoue pour une adresse donnée, la fonction perror() est utilisée pour afficher un message d'erreur spécifique à la création du socket.


### Read Request - RRQ :

d'abord on vérifie si le premier argument passé en ligne de commande est égal à "gettftp". Si c'est le cas, cela signifie que l'utilisateur veut récupérer un fichier TFTP, donc on calcule la longueur nécessaire pour construire la requête RRQ en prenant en compte la longueur du nom du fichier ("file"), la longueur du mode ("mode"), et un ajout de 4 pour les octets nécessaires pour le codage du type de paquet (Opcode) et les octets nuls terminaux. On Alloue dynamiquement de la mémoire pour stocker la requête RRQ en utilisant la taille calculée.

#### Construction la requête RRQ dans la mémoire allouée :

- Place les opcodes spécifiques pour une requête de lecture (Opcode 1 pour RRQ).
- Copie le nom du fichier et ajoute un octet nul à la fin pour indiquer la fin du nom du fichier.
- Copie le mode et ajoute également un octet nul à la fin pour indiquer la fin du mode.


#### Option blocksize est présent : 
- Copie "blksize" à l'emplacement approprié dans la requête RRQ.
- Ajoute un octet nul après "blksize".
- Copie la taille du bloc (block_size) à la position correspondante dans la requête RRQ.
- Ajoute également un octet nul après la taille du bloc.

### Renvoi de la requête : 

sendto() : envovois des données via un socket  (UDP dans). Elle prend plusieurs paramètres :
- sockfd : Le descripteur de socket sur lequel envoyer les données.
- RRQ : Le pointeur vers le buffer contenant les données à envoyer.
- len : La longueur des données à envoyer.
- (struct sockaddr *)current->ai_addr : L'adresse du destinataire, spécifiée dans ai_addr de l'élément current de la liste des adresses obtenues précédemment.
- current->ai_addrlen : La longueur de l'adresse du destinataire.

l'envoi la requête de lecture (RRQ) construite précédemment au serveur via un socket UDP et affiche le nombre d'octets envoyés pour cette requête. Si une erreur se produit lors de l'envoi, un message d'erreur approprié s'affichera.

### Réception de fichier.
recvfrom() : Cette fonction est utilisée pour recevoir des données à partir du socket. Elle prend plusieurs arguments :

- sockfd : Le descripteur de socket sur lequel recevoir les données.
- buf : Le pointeur vers le tampon où stocker les données reçues.
- block_size_value + 4 : La taille maximale des données à recevoir.
- 0 : Aucun flag est utilisé.
(struct sockaddr *)&addr : L'adresse du socket de l'expéditeur, mise à jour avec les informations de l'expéditeur.
- &slen : La taille de la structure d'adresse.

### Ecriture de fichier récupéré : 

Utilisation de la fonction fwrite() pour écrire les données reçues dans le fichier ouvert précédemment (downloaded_file). Le pointeur buf + 4 indique le début des données dans buf après les octets d'en-tête, et data_received spécifie le nombre d'éléments de taille sizeof(char) (c'est-à-dire 1 octet) à écrire dans le fichier.

### Acquittement 

*ACK = htons((u_int16_t)4); : Place le code d'opération (Opcode) 4 dans le premier mot de 16 bits de l'ACK. Dans le protocole TFTP.
*(ACK + 1) = htons((u_int16_t)*(buf + 3)); : Place le numéro de bloc du paquet reçu dans le deuxième mot de 16 bits de l'ACK. Le numéro de bloc est extrait du quatrième octet (indice 3) du paquet de données reçu (buf).

if (*(buf + 1) == 6) { ... } : Vérifie si le deuxième octet (indice 1) du paquet reçu (buf) correspond à un code d'opération 6, qui dans le protocole TFTP représente un paquet d'ACK.

Si le paquet reçu est un paquet d'ACK (le code d'opération est 6) :

- *(ACK + 1) = htons((u_int16_t)0); : Place le numéro de bloc 0 dans l'ACK, indiquant l'ACK pour le premier paquet.
- Envoie l'ACK au serveur à l'aide de sendto().
- Affiche un message indiquant l'envoi de l'ACK pour le premier paquet.
- Initialise data_received à la valeur de block_size_value.
- Sinon (si le paquet reçu n'est pas un paquet d'ACK) :

     - Envoie l'ACK au serveur à l'aide de sendto() sans modifier le numéro de bloc dans l'ACK.
     - Affiche un message indiquant l'envoi de l'ACK pour le numéro de bloc spécifié dans le paquet reçu (*(buf + 3)).

































