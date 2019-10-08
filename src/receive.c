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

int print = 0;

int IPV6_translater(const char* hostname, struct sockaddr_in6 *ipv6){
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
    ipv6 = (struct sockaddr_in6 *)ai->ai_addr;

    if(print == 0){
    void *addr;
    char ipstr[INET6_ADDRSTRLEN];
    addr = &(ipv6->sin6_addr);
    char ipver = '6';
    inet_ntop(ai->ai_family, addr, ipstr, sizeof ipstr);
    printf(" IPv%c: %s\n", ipver, ipstr);
    }

    freeaddrinfo(ai);
    return 0;
}

int socket_init(struct sockaddr_in6 *src_addr, const int src_port,
                struct sockaddr_in6 *dest_addr, const int dest_port){

    int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP); // IPv6, datagrams, UDP
    if(sockfd == -1){fprintf(stderr, "[socket_init] Impossible de créer un socket.\n"); return -1;}

    // Bind l'adresse source avec le socket
    if(src_addr != NULL && src_port > 0){
        src_addr->sin6_port = htons(src_port);
        if(bind(sockfd,(const struct sockaddr *)src_addr, (socklen_t)sizeof(struct sockaddr_in6)) < 0){ fprintf(stderr, "[socket_init] Impossible de bind la source \n"); return -1;}
    }

    // Connect l'adresse de destination avec le socket
    if(dest_addr != NULL && dest_port > 0){
        dest_addr->sin6_port = htons(dest_port);
        if(connect(sockfd,(const struct sockaddr *)dest_addr, (socklen_t)sizeof(struct sockaddr_in6)) < 0){ fprintf(stderr, "[socket_init] Impossible de connect la destination \n"); return -1;}
    }

    return sockfd;
}

int receive(char* hostname, int port, char* filename){
    return EXIT_SUCCESS;
}
