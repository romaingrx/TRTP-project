#include <netinet/in.h>



int socket_listening(char* hostname, int port, int nombremaxdeconnections, char * main_format, int boolean_window);

int new_client();

void free_receive();
/*
 * Transforme l'adresse en IPv6
 * @return : -1 si une erreur survient
 *           sinon initialise la structure ipv6 et retourne 0
 */

int IPV6_translater(const char* hostname, struct sockaddr_in6 *ipv6);

/*
 * Crée un socket, bind la source et connect la destination
 * @return : -1 si une erreur survient
 *           sinon retourne le file descriptor du socket
 */

 int socket_init(struct sockaddr_in6 *src_addr, const int src_port,
                 struct sockaddr_in6 *dest_addr, const int dest_port);




/*
 * Écrit dans le fichier correspondant à l'indice du client
 * @return : -1 si une erreur survient
 *           sinon le nombre de byte écrit
 */
// static int writeFile(const int index, const char * buf, size_t len);
