#include <netinet/in.h>

/*
 * Initialise le socket
 * @return : -1 si une erreur survient
 *          le file descriptor du socket sinon
 *
 */

static int socket_init();


/*
 * Transforme l'adresse en IPv6
 * @return : -1 si une erreur survient
 *           sinon initialise la structure ipv6 et retourne 0
 *
 */

int IPV6_translater(const char* hostname, struct sockaddr_in6 *retipv6);
