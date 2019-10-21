#include "packet.h"
#include "queue.h"
#include "receive.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // write, close
// #include <sys/types.h>
// #include <sys/stat.h>
#include <string.h> // memset
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo
#include <errno.h>
#include <unistd.h>//read()
#include <fcntl.h> // open
#include <stdbool.h> // bool
#include <arpa/inet.h>


//Tuto qui nous a aidé: https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
int print = 0;
int iterator_file = 0, len_format = 0, max_connections; // max_connections == -1 si pas de limite
char * format = NULL;
bool MAX = true;




// All needed variables for socket_listening.



fd_set readfds;

int new_client(){
    if(realloc(file_descriptors, len_format * clients_known) == NULL){fprintf(stderr, "[NEW_CLIENT] malloc: %s", strerror(errno));return -1;}
    if(realloc(clients, sizeof(struct sockaddr_in6)*clients_known) == NULL){fprintf(stderr, "[NEW_CLIENT] malloc: %s", strerror(errno));return -1;}
    return openFile();
}

void free_receive(){
    if(file_descriptors != NULL){free(file_descriptors);}
    if(clients != NULL){free(clients);}
}

int create_master_socket(int * master_socket, char * hostname, int port, int * addrlen){
    struct sockaddr_in6 address;
    *master_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    address.sin6_family = AF_INET6;
    address.sin6_flowinfo = 0;
    if(hostname == NULL) address.sin6_addr=in6addr_any;
    else inet_pton(AF_INET6, hostname, (void *)&address.sin6_addr.s6_addr);
    address.sin6_port =htons(port);
    address.sin6_scope_id = 0;
    if (bind(*master_socket, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
            fprintf(stderr, "Error binding master socket: %s", strerror(errno));
            return -1;
    }
    *addrlen = sizeof(address);
    return 0;
}


int treat_message_from(struct sockaddr_in6 address, char* buffer, int bufsize){
  if(clients == NULL){
    clients = malloc(sizeof(struct sockaddr_in6));
    file_descriptors = malloc(len_format);
    if(openFile()==-1){fprintf(stderr, "[treat_message_from] openfile: %s", strerror(errno));return -1;}
    memcpy(&clients[0], &address, sizeof(struct sockaddr_in6));
    printf("Added client 0\n");
  }

  for(int i = 0; i<clients_known; i++){
    char adr1[INET6_ADDRSTRLEN];
    char adr2[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6,&clients[i].sin6_addr, adr1, INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6,&address.sin6_addr, adr2, INET6_ADDRSTRLEN);
    if(strcmp(adr1,adr2)==0){
      //Ici on a recu le message buffer du client indice i;
      printf("Received message from client %d:::%s\n",  i, buffer);
      if(treat_bytestream(buffer, 1024, i) ==2){
        //Treats bytestream. If it returns 2, this was the last message of the connection.
        //I can thus remove this client_known.
      }


      return 0;
    }
  }
  //On a pas trouvé dans le tableau, il faut rajouter du coup.
  if ((!MAX) || (MAX && clients_known<max_connections)) {
      clients_known++;
      if(new_client() == -1){
          fprintf(stderr, "[treat_message_from] : %s\n", strerror(errno));
          return -1;
      }
      // clients = realloc(clients, sizeof(struct sockaddr_in6)*clients_known);
      memcpy(&clients[clients_known-1], &address, sizeof(struct sockaddr_in6));
  }
  return 0;
}

int socket_listening(char* hostname, int port, int nombr, char * main_format){
    init_queue(nombr);
    max_connections = nombr;
    if(nombr == -1){
        MAX = false;
    }
    format = main_format;
    len_format = strlen(format) + 4;
    clients_known = 1;
    int addrlen;
    char buffer[1024];
    printf("Socket listening\n");


  if (create_master_socket(&master_socket, hostname, port, &addrlen) == -1){
      fprintf(stderr, "Create master socket failed: %s\n", strerror(errno));
      return -1;}


  //WHILE LOOP
  while(clients_known > 0)
   {

      memset(&buffer, 0, 1024);
       //clear the socket set
       FD_ZERO(&readfds);
       //add master socket to set
       FD_SET(master_socket, &readfds);
       int max_sd = master_socket;

       int activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

       if ((activity < 0) && (errno!=EINTR))
       {
           printf("select error");
       }


       //If something happened on the master socket ,
       //then its an incoming connection
       if (FD_ISSET(master_socket, &readfds))
       {
           struct sockaddr_in6 newaddress;
           int bytesread = recvfrom(master_socket, buffer, 1024,MSG_WAITALL, (struct sockaddr*) &newaddress, (socklen_t*)&addrlen);
           if(bytesread <0){
             fprintf(stderr, "[SOCKET LISTENING] recvfrom: %s\n", strerror(errno));
           }
           treat_message_from(newaddress, buffer, 1024);
           printf("Received: %s\n", buffer);



           //ICI JE CONNECTE NEWSOCKET A LA SOURCE
           //int errconnect  = connect(master_socket,(struct sockaddr *) &newaddress, addrlen);
           // printf("Errconnect returns: %d and err: %s\n", errconnect,strerror(errno));
           // char* message = "Bill Gates te remercie\n";
           // if(send(master_socket, message, strlen(message), 0) <1)
           // {
           //     printf("Send error: %s\n",strerror(errno));
           // }


       }
 }
 free_receive();
 free_queue();
 closeFiles();
 return 0;
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




int openFile(){
    char filename[len_format];
    snprintf(filename, len_format, format, clients_known-1);
    int filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 700);
    if(filefd == -1){fprintf(stderr, "[openFile] : %s\n", strerror(errno)); return -1;}
    printf("Nouveau file descriptor : %d\n", filefd);
    file_descriptors[clients_known-1] = filefd;
    return 0;
}

int closeFiles(){
    for (size_t i = 0; i < clients_known; i++) {
            close(file_descriptors[i]);
    }
    return 0;
}

// int writeFile(const int connection, const char * buf, size_t len){
//     return write(file_descriptors[connection], buf, len);
// }

// int printfile(char * filename, char * data, int len, int connection){
//     int fd, err;
//     fd = open(open("foo.txt", O_RDONLY | O_CREAT);  )
// }
