#include "packet_interface.h"
#include "queue.h"
#include "receive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo

#include <arpa/inet.h>

int loge = 0;

int IPV6_translater(const char* hostname, struct sockaddr_in6 *retipv6){
    int status;
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET6; // IPv6
    hints.ai_socktype = SOCK_DGRAM; // Datagrams
    hints.ai_protocol = IPPROTO_UDP; // UDP
    hints.ai_flags    = 0; // Pas de spécificité
    if ((status = getaddrinfo(hostname , NULL, &hints, &ai )) != 0) {
        fprintf(stderr, "[IPV6] getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai->ai_addr;

    if(loge == 0){
    void *addr;
    char ipstr[INET6_ADDRSTRLEN];
    addr = &(ipv6->sin6_addr);
    char ipver = '6';
    inet_ntop(ai->ai_family, addr, ipstr, sizeof ipstr);
    printf(" IPv%c: %s\n", ipver, ipstr);
    }

    retipv6 = ipv6;
    freeaddrinfo(ai);
    return 0;
}

int receive(char* hostname, int port, char* filename){
    return EXIT_SUCCESS;
}
