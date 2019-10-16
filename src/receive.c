#include "packet_interface.h"
#include "queue.h"
#include "receive.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // write, close
// #include <sys/types.h>
// #include <sys/stat.h>
#include <fcntl.h> // open
#include <string.h> // memset
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo
#include <errno.h>
#include <arpa/inet.h>

int print = 0;
int iterator_file = 0, len_format = 0, max_connections; // max_connections == -1 si pas de limite
char * format = NULL;
int * file_descriptors = NULL;



int IPV6_translater(const char* hostname, struct sockaddr_in6 *ipv6){
    int status;
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET6; // IPv6
    hints.ai_socktype = 0; // Datagrams
    hints.ai_protocol = IPPROTO_UDP; // UDP
    hints.ai_flags    = 0; // Pas de spécificité
    if ((status = getaddrinfo(hostname , NULL, &hints, &ai )) != 0) {
        fprintf(stderr, "[IPV6] getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }
    ipv6 = (struct sockaddr_in6 *)ai->ai_addr;
    char *ipstr = malloc(INET6_ADDRSTRLEN);

    void *addr;
    addr = &(ipv6->sin6_addr);
    char ipver = '6';
    inet_ntop(ai->ai_family, addr, ipstr, sizeof ipstr);
    printf(" IPv%c: %s\n", ipver, ipstr);

    inet_pton(AF_INET6, ipstr, &(ipv6->sin6_addr));

    freeaddrinfo(ai);
    return 0;
}

int socket_init(struct sockaddr_in6 *src_addr, const int src_port,
                struct sockaddr_in6 *dest_addr, const int dest_port){


    int sockfd = socket(AF_INET6, SOCK_DGRAM, 0); // IPv6, datagrams, UDP
    if(sockfd == -1){fprintf(stderr, "[socket_init] Impossible de créer un socket.\n"); return -1;}

    // Bind l'adresse source avec le socket
    // if(src_addr != NULL && src_port > 0){
    //     src_addr->sin6_port = htons(src_port);
    //     if(bind(sockfd,(const struct sockaddr *)src_addr, sizeof(struct sockaddr_in6)) < 0){ printf("%s\n", strerror(errno));fprintf(stderr, "[socket_init] Impossible de bind la source \n"); return -1;}
    // }

    // Connect l'adresse de destination avec le socket
    if(dest_addr != NULL && dest_port > 0){
        dest_addr->sin6_port = htons(dest_port);
        if(connect(sockfd,(const struct sockaddr *)dest_addr, (socklen_t)sizeof(struct sockaddr_in6)) < 0){ fprintf(stderr, "[socket_init] Impossible de connect la destination \n"); return -1;}
    }

    return sockfd;
}


int receive(int connections, char * hostname, int port, char * main_format){
    max_connections = connections;
    format = malloc(sizeof(char)*strlen(main_format));
    strcpy(format, main_format);
    len_format = strlen(format) + 4;
    if (max_connections != -1) {
        file_descriptors = malloc(sizeof(int) * max_connections);
    }
    if(file_descriptors!= NULL){free(file_descriptors);}
    return 0;
}

int openFile(){
    char filename[len_format];
    int err = snprintf(filename, len_format, format, iterator_file);
    int filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC);
    if(filefd == -1){fprintf(stderr, "[openFile] : %s\n", strerror(errno)); return -1;}
    file_descriptors[iterator_file] = filefd;
    iterator_file ++;
    return 0;
}

int closeFile(const int index){
    close(file_descriptors[index]);
    return 0;
}

int writeFile(const int index, const char * buf, size_t len){
    return write(file_descriptors[index], buf, len);
}

// int printfile(char * filename, char * data, int len, int connection){
//     int fd, err;
//     fd = open(open("foo.txt", O_RDONLY | O_CREAT);  )
// }
