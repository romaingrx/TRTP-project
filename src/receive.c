#include "packet_interface.h"
#include "queue.h"
#include "receive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo
#include <errno.h>
#include <unistd.h>//read()
#include <fcntl.h> // for open

#include <arpa/inet.h>


//Tuto qui nous a aidé: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
int print = 0;

//All needed variables for socket_listening.
int master_socket, addrlen, *client_socket, max_clients = 10, activity, i, valread, sd, max_sd;
struct sockaddr_in6 address, newaddress;
char buffer[513];
fd_set readfds;


void free_receive(){
  free(client_socket);
}

int socket_listening(char* hostname, int port, int n_connections){
  printf("Socket listening\n");
  client_socket = malloc(sizeof(int)*n_connections);

  //Initialising all client sockets to 0
  for (i = 0; i < max_clients; i++)   { client_socket[i] = -1;  }

  //CREATING AND INITIALISING A MASTER SOCKET
  if( (master_socket = socket(AF_INET6 , SOCK_DGRAM , 0)) == 0)
  {
      perror("socket failed");
      return -1;
  }
  address.sin6_family = AF_INET6;
  if(hostname == NULL) address.sin6_addr=in6addr_any;
  else inet_pton(AF_INET6, hostname, (void *)&address.sin6_addr.s6_addr);
  address.sin6_port =htons(port);
  address.sin6_scope_id = 0;
  if (bind(master_socket, (struct sockaddr*) &address, sizeof(address)) < 0)
  {
          fprintf(stderr, "Error binding master socket: %s", strerror(errno));
          return -1;
  }
  addrlen = sizeof(address);
  //END OF INITIALISING MASTER socket
  //WHILE LOOP
  while(1)
   {
       //clear the socket set
       FD_ZERO(&readfds);
       //add master socket to set
       FD_SET(master_socket, &readfds);
       max_sd = master_socket;

       //add child sockets to set
       for ( i = 0 ; i < max_clients ; i++)
       {
           //socket descriptor
           sd = client_socket[i];
           //if valid socket descriptor then add to read list
           if(sd > 0)
               FD_SET( sd , &readfds);

           //highest file descriptor number, need it for the select function
           if(sd > max_sd)
               max_sd = sd;
       }

       //wait for an activity on one of the sockets , timeout is NULL ,
       //so wait indefinitely

       activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

       if ((activity < 0) && (errno!=EINTR))
       {
           printf("select error");
       }


       //If something happened on the master socket ,
       //then its an incoming connection
       if (FD_ISSET(master_socket, &readfds))
       {
           int bytesread = recvfrom(master_socket, buffer, 513,MSG_WAITALL, (struct sockaddr*) &newaddress, &addrlen);
           if(bytesread <0){
             printf("Error reading: %s\n", strerror(errno));
           }

           printf("Received: %s", buffer);



           // //ICI JE RENVOIE JUSTE UN MESSAGE A LA SOURCE
           char* message = "Bill Gates te remercie\n";
           // if(sendto(master_socket, message, strlen(message), 0,(struct sockaddr*)&newaddress, addrlen) <1)
           // {
           //     printf("Send error: %s\n",strerror(errno));
           // }

           //ICI JE CONNECTE NEWSOCKET A LA SOURCE
           int errconnect  = connect(master_socket,(struct sockaddr *) &newaddress, addrlen);
           // printf("Errconnect returns: %d and err: %s\n", errconnect,strerror(errno));

           //EN THEORIE ICI JENVOIE A LA SOURCE, IL RETOURNE QU'il Y ARRIVE MAIS IL S'AVERE QUE LA SRC NE RECOIT RIEN
           if(send(master_socket, message, strlen(message), 0) <1)
           {
               printf("Send error: %s\n",strerror(errno));
           }
           else printf("RESPONSE SENT\n");

           //add new socket to array of sockets
           for (i = 0; i < max_clients; i++)
           {
               //if position is empty
               if( client_socket[i] == -1 )
               {
                   client_socket[i] = master_socket;
                   printf("Adding to list of sockets as %d\n" , i);

                   i = max_clients +1;
               }
           }
       }

       //else its some IO operation on some other socket
       for (i = 0; i < max_clients; i++)
       {
           sd = client_socket[i];

           if (FD_ISSET(sd, &readfds))
           {
               //Check if it was for closing , and also read the
               //incoming message
               if ((valread = read( sd , buffer, 1024)) == 0)
               {
                  //  //Somebody disconnected , get his details and print
                  //  getpeername(sd , (struct sockaddr*)&address , \
                  //      (socklen_t*)&addrlen);
                  //  printf("Host disconnected , ip %s , port %d \n" ,
                  // inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                  //
                  //  //Close the socket and mark as 0 in list for reuse
                  //  close( sd );
                  //  client_socket[i] = 0;
               }

               //Echo back the message that came in
               else
               {
                   //set the string terminating NULL byte on the end
                   //of the data read
                   buffer[valread] = '\0';
                   send(sd , buffer , strlen(buffer) , 0 );
               }
           }
       }
   }

  //END OF WHILE LOOP




}

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

int receive(char* hostname, int port, char* filename){
    return EXIT_SUCCESS;
}
