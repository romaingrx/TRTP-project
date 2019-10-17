#include "packet.h"
#include "queue.h"
#include "receive.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int main(int argc, char* argv){
//   int status;
//   struct addrinfo hints, *ai;
//   memset(&hints, 0, sizeof(hints));
//   hints.ai_family   = AF_INET6; // IPv6
//   hints.ai_socktype = SOCK_DGRAM; // Datagrams
//   hints.ai_protocol = IPPROTO_UDP; // UDP
//   hints.ai_flags    = 0; // Pas de spécificité
//   if ((status = getaddrinfo("::1" , NULL, &hints, &ai )) != 0) {
//       fprintf(stderr, "[IPV6] getaddrinfo: %s\n", gai_strerror(status));
//       return -1;
//   }
//   struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai->ai_addr;
//
//   int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
//   if(sockfd == -1){printf("Nique ta mere");}
// int len;
//   sendto(sockfd, "Hello", strlen("hello"), MSG_CONFIRM,(struct sockaddr *) ipv6,sizeof(ipv6))    ;
//   printf("Message envoyé");
//
//   freeaddrinfo(ai);
int sockfd;
char buffer[1024];
char *hello = "Hello from client";
struct sockaddr_in6	 servaddr;

// Creating socket file descriptor
if ( (sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0 ) {
  perror("socket creation failed");
  exit(EXIT_FAILURE);
}

memset(&servaddr, 0, sizeof(servaddr));

// Filling server information
// servaddr.ai_family = AF_INET6;
// servaddr.ai_port = htons(6060);
// servaddr.sin_addr.s_addr = INADDR_ANY;

servaddr.ai_family   = AF_INET6; // IPv6
servaddr.ai_socktype = SOCK_DGRAM; // Datagrams
servaddr.ai_protocol = IPPROTO_UDP; // UDP
servaddr.ai_flags    = 0; // Pas de spécificité

int n, len;

sendto(sockfd, (const char *)hello, strlen(hello),
  MSG_CONFIRM, (const struct sockaddr *) &servaddr,
    sizeof(servaddr));
printf("Hello message sent.\n");
close(sockfd);


  return 0;
}
