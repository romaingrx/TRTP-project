
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // write, close
#include <stdbool.h>


extern int master_socket;
extern struct sockaddr_in6* clients;
extern int * file_descriptors;
extern int clients_known; //Clients actuellement occupés de travailler
extern int log_out;
extern int n_connections; //Nombre maximal de connections
extern bool MAX;
extern char * format;
extern int len_format;
extern int nb_file;
extern int timeout;
extern int adapt_window; //1 adapts window, 0 doesn't. Adapting according to transmission errors
// Prend un paquet et une connection en argument et les Renvoie au def
// dans un buffer qui devra en faire un fichier
void data_ind(pkt_t *pkt, int connection);


pkt_status_code treat_bytestream(char* data, size_t len, int connection);



//Envoie un ack avec pour numéro de séquence n à la connection
//correspondante

//ATTENTION ELLE NE MARCHE PAS ENCORE HOHO
void send_ack(uint8_t n, uint32_t time,int connection, ptypes_t type);


//Initialise cette interface, à appeler dans la main
//@n: nombre de connections entrantes
int init_queue(int n);

//Ajoute une connection entrante. ATTENTION, il faut appeler define_connection juste après
//Exemple d'utilisation:
// init queue(2);
// define_connection(0,3);
// define_connection(1,4)
// int add_queue();
// define_connection(get_nconnections-1, windowsize );
int add_queue();

//Getter de la variable n connections
int get_nconnections();

//For testing:
pkt_t* pkt_generate(int seq, int wid);

//Definit la taille de window d'une des connections,
//et initialise toutes les autres variables internes de cette connection.
//@con_indice: le numéro de la connection à initialiser (0,1,2,..,n-1)
//@window_Size: la taille de la fenêtre de cette connection
int define_connection(int con_indice);

//Free la queue, à appeler en fin d'exécution
//free aussi chaque buffer interne
void free_queue();

//LA PERLE NOIRE de cette classe
//prend un **BYTESTREAM** en entrée, et un numéro de connection
//Verifie si le packet est valide en appelant packet_implem
//fait en sorte que le paquet sera data_ind dans le bon ordre.
//appelle send_ack lorsque le protocole selective repeat l'impose
//@pkt: le paquet, sous forme de char*
//@connection: l'indice de la connections
//REMARQUE: pour le moment on prend un pkt en entrée.
int data_req(pkt_t *pkt, int connection);



//Ajoute un pkt_t dans le buffer de packets.
// Cette fonction ajoute le packet tel que leurs SEQNUM soient ordonnés
//Ainsi, le premier pkt sera celui avec le plus petit indice.
//@pkt: le paquet en question
//@connection: l'indice de la connection en cours.
void buffer_add(pkt_t *pkt, int connection);


//Renvoie le premier packet sur le buffer.
//Null si head==Null
//@connection: indice//@connection: indice de la connection de la connection
pkt_t* buffer_peak(int connection);

//Retire le premier packet du buffer et le free.
//Appeler buffer_peak pour récuperer ce paquet avant de le supprimer
//@connection: indice de la connection
void buffer_remove(int connection);

/*
* Free le buffer, cette fonction est appelée par free_queue()
*/
void buffer_free(int connection);

//Incrémente la variable next d'une connections
//En tenant compte des limtes en indices (2¨n)
//@connection: indice de la connection
void next_inc(int connection);


//Si t'as lu cette ligne de commentaire je te chatouillerai
//le pied gauche hihi
void window_inc(int connection);

/*
 * Crée un file descriptor pour le fichier de sortie et le stock dans une
 * variable globale à l'indice correspondant au client.
 * @return : -1 si une erreur survient
 *           sinon 0
 */
int openFile(int i);

/*
 * Ferme le file descriptor correspondant à l'indice du client.
 * @return : -1 si une erreur survient
 *           sinon 0
 */
int closeFiles();

/*
 * Réarrange tous les tableaux.
 * @return : 0
 */
int rearange_tabs(int connection);
