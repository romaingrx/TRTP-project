#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "packet.h"
#include "queue.h"
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <unistd.h> // write, close
#include <errno.h>
#include <unistd.h>//read()
#include <fcntl.h>
#include <stdbool.h>

int log_out; //Makes the program log to stdout

//Global, extern variables:
int master_socket; //The socket for receiving
struct sockaddr_in6* clients; //Saves the IP adresses of known clients
int * file_descriptors = NULL; //Stores the file descriptors for the known clients
int clients_known; //Number of clients CURRENTLY known
int n_bits_encode_window = 8; //should remain constant
int n_connections = -1; //Number of maximal connections (given as argument to main.)
char * format = NULL;
bool MAX = true;
int len_format = 0;
int nb_file = 0;
int adapt_window;
int timeout;



int get_nconnections(){
  return n_connections;
}
typedef struct node {
    pkt_t* data;
    struct node * next;
} node_t;

//The following variables are arrays that store the variables for EACH connection con_indice
//windowsize[] = {1,2,3,5} means connection 0 has window of 1, con 1 of 2, con 2 of 3, con 3 of 5.
int* windowsize = NULL;
uint8_t* lastackn=NULL;
uint32_t* lastackt = NULL;
int* next=NULL;

int* window_start=NULL;
int* window_end=NULL;

node_t** head;


int invalid_packets = 0;

void got_invalid(int connection){
  if(!adapt_window)return;
  invalid_packets++;

  if(invalid_packets > 50){
    invalid_packets = 0;
    windowsize[connection] = windowsize[connection]/2;
    if(windowsize[connection] == 1){
      windowsize[connection]=2;
    }
    int wend = window_start[connection] + windowsize[connection];
    wend = wend%255;
    window_end[connection] = wend;
    if(log_out)printf("WINDOW ADAPT -- : [%d,%d]\n", window_start[connection], window_end[connection]);
  }
}
void got_valid(int connection){
  if(!adapt_window)return;
  invalid_packets--;

  if(invalid_packets < 1){
    invalid_packets = 0;
    windowsize[connection] = windowsize[connection]*2;
    if(windowsize[connection] > 30){
      windowsize[connection]=30;
    }
    int wend = window_start[connection] + windowsize[connection];
    wend = wend%255;
    window_end[connection] = wend;
    if(log_out)printf("WINDOW ADAPT++: [%d,%d]\n", window_start[connection], window_end[connection]);
  }
}
//Call this function, by giving it a malloc'd packet and a connection Number
//This function will write the payload to the file descriptor associated to the connection indice
//Frees the packet afterwards
void data_ind(pkt_t *pkt, int connection){

  if(file_descriptors[connection] == -1){
      if(openFile(connection) == -1){return;}
  }

  if(log_out){
  printf("Successfully recieved data on connection %d, number: %d\n",connection,pkt->SEQNUM);}
  // printf("PAYLOAD %s\n", pkt_get_payload(pkt));}
  if(write(file_descriptors[connection], pkt_get_payload(pkt), pkt_get_length(pkt)) == -1){
      fprintf(stderr, "[data_ind] : %s\n", strerror(errno));
  }
  pkt_del(pkt);
}






//This function initialises a connection and all its variables
int define_connection(int con_indice){
  int window_size = 30; //default
  windowsize[con_indice] = window_size;
  lastackn[con_indice]=-1;
  lastackt[con_indice]=0;
  next[con_indice]=0;
  window_start[con_indice]=0;
  window_end[con_indice]=window_size-1;
  head[con_indice]=NULL;
  return 0;
}

//This function initialises a queue (malloc's the array of varaible. HAS TO BE CALLED at the start of program)
int init_queue(int n){
  int* err;
  err = (int*)malloc(sizeof(int)*n);
  if(err==NULL){
    fprintf(stderr,"[init_queue] malloc erreur");
    return(-1);
  }
  windowsize = err;

  lastackn = (uint8_t*)malloc(sizeof(uint8_t)*n);
  if(lastackn==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  lastackt = (uint32_t*)malloc(sizeof(uint32_t)*n);
  if(lastackt==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  err = (int*)malloc(sizeof(int)*n);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  next = err;

  err = (int*)malloc(sizeof(int)*n);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  window_start = err;

  err = (int*)malloc(sizeof(int)*n);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  window_end = err;
  head = (node_t**)malloc(sizeof(node_t*)*n);
  if(head==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  file_descriptors = (int*)malloc(sizeof(int)*n);
  if(file_descriptors==NULL){
    fprintf(stderr,"[queue_init] : %s", strerror(errno));
    return(-1);
  }

  // printf("DATA DATA DATA %d\n", file_descriptors[0]);

  for(int i=0; i<n; i++){
    file_descriptors[i] = -1;
    windowsize[i]=4; //Should be changed by program
    lastackn[i]=-1;
    lastackt[i]=0;
    next[i]=0;
    window_start[i]=0;
    window_end[i]=0;
    define_connection(i);
  }
  return 0;
}


int add_queue(){
  int oldn = clients_known;
  clients_known++;

  int* err;
  err = (int*)realloc(windowsize, sizeof(int)*clients_known);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  windowsize = err;

  lastackn= (uint8_t*)realloc(lastackn, sizeof(uint8_t)*clients_known);
  if(lastackn==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  lastackt= (uint32_t*)realloc(lastackt, sizeof(uint32_t)*clients_known);
  if(lastackt==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  err = (int*)realloc(next, sizeof(int)*clients_known);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  next = err;

  err = (int*)realloc(window_start, sizeof(int)*clients_known);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  window_start = err;

  err = (int*)realloc(window_end, sizeof(int)*clients_known);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  window_end = err;


  head = (node_t**)realloc(head, sizeof(node_t*)*clients_known);
  if(head==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  file_descriptors = (int*)realloc(file_descriptors, sizeof(int)*clients_known);
  if(file_descriptors==NULL){
    fprintf(stderr,"[add_queue] : %s", strerror(errno));
    return(-1);
  }


    int i = oldn;
    windowsize[i]=4; //Should be changed by program
    lastackn[i]=-1;
    lastackt[i]=0;
    next[i]=0;
    window_start[i]=0;
    window_end[i]=0;
    define_connection(i);


  return 0;
}





//BUFFER IMPLEMENTATION
//Adds a node to the buffer. The nodes are stored in increasing seqnum order.
void buffer_add(pkt_t *pkt, int connection){
  node_t *newnode = (node_t*)malloc(sizeof(node_t));
  int n = pkt->SEQNUM;

  if(newnode ==NULL){
    fprintf(stderr,"[Malloc failed in buffer_add]");
  }
  newnode->data = pkt;
  newnode->next = NULL;
  if(head[connection]==NULL){
    head[connection]=newnode;
  }
  else{
    if(head[connection]->data->SEQNUM >n){
      newnode->next = head[connection];
      head[connection]=newnode;

    }
    else{
    node_t *runner = head[connection];
    while(runner->next!=NULL && runner->next->data->SEQNUM <n){
      runner = runner->next;
    }
    newnode->next = runner->next;
    runner->next = newnode;
  }}
  if(log_out){printf("ADDED %d to buffer of connection %d\n", newnode->data->SEQNUM, connection);}
}
//Looks at the head of the buffer
pkt_t* buffer_peak(int connection){
  if(head[connection]==NULL){
    return NULL;
  }
  pkt_t* ret = head[connection]->data;
  return ret;
}
//Removes the head from the buffer
void buffer_remove(int connection){
  node_t* oldhead = head[connection];
  head[connection] = head[connection]->next;
  free((void*)oldhead);
}
//Frees the whole buffer of a connection
void free_buffer(int connection){
  while(head[connection] != NULL){
    pkt_del(head[connection]->data);
    node_t* oldhead = head[connection];
    head[connection]=oldhead->next;
    free(oldhead);
  }
}

//END OF BUFFER


void free_queue(){
  for(int i =0; i<n_connections;i++){
    free_buffer(i);
  }
  free(windowsize);
  free(lastackn);
  free(lastackt);
  free(next);
  free(window_start);
  free(window_end);
  free(head);
}


void next_inc(int connection){
  if(next[connection] < pow(2,n_bits_encode_window)-1){
  next[connection]++;
  return;
  }
  next[connection] = 0;
  // next[connection]++;
  return;
}

//Shitfts the window of 1 to the right on a given connection
void window_inc(int connection){
  if(window_start[connection] < pow(2,n_bits_encode_window)-1){window_start[connection] ++;}
  else {window_start[connection] = 0;}


  if(window_end[connection] < pow(2,n_bits_encode_window)-1){ window_end[connection] ++;}
  else {    window_end[connection] = 0;}
  if(log_out){

  printf("[%d,%d]\n", window_start[connection], window_end[connection]);}
}

//Sends an ack packet with the given arguments to the socket associated to the given connecton number.
void send_ack(uint8_t n, uint32_t temps,int connection, ptypes_t type){

  if(window_start[connection] == n){
    window_inc(connection);
  }
  lastackn[connection] = n;
  lastackt[connection] = temps;


  if(head[connection] != NULL){
    if(head[connection]->data->SEQNUM ==  n+1){
      return;
    }
  }
  //The little if head connection != null makes sure only the last ack is being sent when several
  //packets are stored in the buffer.



  uint8_t toack = 0;
  if(n != 255){toack = n+1;}

  if(log_out)
  {
    if(type  == PTYPE_ACK)  printf("ACK %d timestamp: %u\n", toack,temps);
    if(type == PTYPE_NACK)  printf("NCK %d timestamp: %u\n", toack,temps);
  }



  //Gere i'm generatine a packet which I'll send via sockets.
  pkt_t* ackpacket = pkt_new();
  pkt_set_type(ackpacket,type);
  pkt_set_tr(ackpacket, 0);
  pkt_set_window(ackpacket, windowsize[connection]);
  if(type == PTYPE_NACK)toack--;
  pkt_set_seqnum(ackpacket, toack);
  pkt_set_length(ackpacket,0);
  pkt_set_timestamp(ackpacket,temps);

  size_t len = 1024;
  char* donnees = malloc(sizeof(char)*len);
  pkt_encode(ackpacket, donnees, &len); //always returns PKT_OK

  //TODO: Envoyer donnees via sockets, mais faut la socket et
  //la bonne addresse
  struct sockaddr_in6 theone = clients[connection];
  if(  sendto(master_socket, donnees, len, 0,(struct sockaddr*)&theone, sizeof(struct sockaddr_in6)) == -1){
    fprintf(stderr, "[SEND ACK] error using sendto: %s \n", strerror(errno));
  }

  free(donnees);







}
int rearange_tabs(int connection){
    if(close(file_descriptors[connection]) == -1){
        fprintf(stderr, "[rearange_tabs] : %s\n", strerror(errno));
    }
    if(clients_known > 1){
        for (size_t i = connection; i < clients_known-1; i++) {
            file_descriptors[i] = file_descriptors[i+1];
            windowsize[i] = windowsize[i+1];
            lastackn[i] = lastackn[i+1];
            lastackt[i] = lastackt[i+1];
            next[i] = next[i+1];
            window_start[i] = window_start[i+1];
            window_end[i] = window_end[i+1];
            head[i] = head[i+1];
	    clients[i] = clients[i+1];
        }
        if ((realloc(windowsize, clients_known-1)==NULL) ||
            (realloc(file_descriptors, clients_known-1)==NULL) ||
            (realloc(lastackn, clients_known-1)==NULL) ||
            (realloc(lastackt, clients_known-1)==NULL) ||
            (realloc(next, clients_known-1)==NULL) ||
            (realloc(window_start, clients_known-1)==NULL) ||
            (realloc(window_end, clients_known-1)==NULL) ||
            (realloc(head, clients_known-1)==NULL)||
	    (realloc(clients, clients_known-1)==NULL)) {
                fprintf(stderr, "[rearange_tabs] %s\n", strerror(errno));
                return -1;
        }
    }else{
        free(windowsize);
        free(lastackn);
        free(lastackt);
        free(next);
        free(window_start);
        free(window_end);
        free(head);
	    free(clients);
        free(file_descriptors);
        init_queue(1);

    }
    clients_known--;
    return 0;
}




//This function treats the selective repeat on a packet, on a given connections
//Return value: -1 if error, 0 if all went well.
int data_req(pkt_t* pkt, int connection){
  //If packet was tronqued
  if(pkt->TR==1){
    send_ack(pkt->SEQNUM, pkt->TIMESTAMP,connection, PTYPE_NACK);
    got_invalid(connection);
  }
  //If variable window size
  // if(pkt->WINDOW != windowsize[connection]){
  //   window_end[connection] = window_start[connection]+pkt->WINDOW -1;
  //   windowsize[connection] = pkt->WINDOW;
  // }


  //Checking if packet is in the receiving window.
  int n = pkt->SEQNUM;
  if(window_start[connection] < window_end[connection]){
    got_invalid(connection);
    if(n < window_start[connection] || n > window_end[connection]){
      //Packet not inside window, ignore it
      if(log_out){
      printf("Out of window packet 1: %d\n", n);}
      pkt_del(pkt);
      return 0;
    }
  }

  if(window_start[connection] > window_end[connection]){
      got_invalid(connection);
    if(pkt->SEQNUM > window_start[connection] || pkt->SEQNUM < window_end[connection]){
      //Packet not inside window, ignore it
      if(log_out)printf("Out of window packet 2 %d \n", pkt->SEQNUM);
      pkt_del(pkt);
      return 0;
    }
  }



  got_valid(connection);
  if(n == next[connection]){
    if(pkt->TR == 0 && pkt->TYPE == PTYPE_DATA && pkt->LENGTH == 0){
      //If entering here, it means this packet was the very last one of the file.
      //I must
      //1/ End the file descriptor
      //2/ Free this connection's buffer
      //3/ Make sure queue has a free spot for any future connections
      //4/ Clear the known address in the clients list
      if(log_out)printf("\n\nGot last packet on connection %d\n",connection);
      send_ack(pkt_get_seqnum(pkt), pkt_get_timestamp(pkt), connection, PTYPE_ACK);
      pkt_del(pkt);
      rearange_tabs(connection);
      return 2;

    }
    //If the packet is in sequence
    next_inc(connection);
    send_ack(pkt->SEQNUM, pkt->TIMESTAMP, connection, PTYPE_ACK);
    data_ind(pkt, connection) ;


    //I recieved a valid packet, now I check in the buffer if I had stored the next Expected
    //packet. If it is in the buffer I call this function recursively with it. Otherwise the function ends.
    //do that by calling data_req(buffer.next) (recursion)
    pkt_t* buf = buffer_peak(connection);
    if(buf != NULL){
      if(buf->SEQNUM == next[connection]){
        buffer_remove(connection);
        data_req(buf, connection);//Peter van Roy tmtc

      }
    }

    return 0;
  }
  else{
    got_invalid(connection);
    //if the packet is out of sequence BUT inside the recieving window:
    //add it to buffer
    if(head[connection] != NULL){
      if(buffer_peak(connection)->SEQNUM == n){
        if(log_out){printf("Already in buffer\n");}
        pkt_del(pkt);
        return 0;
      }
    }
    buffer_add(pkt, connection);
    send_ack(lastackn[connection],lastackt[connection], connection, PTYPE_ACK);
    return 0;
  }
}



//Treats a packet, given as argument in a char*.
//Thus decodes the packets and sends it to data_req.
//Returns 0 if all went well, -1 if errors,
pkt_status_code treat_bytestream(char* data, size_t len, int connection){
  pkt_t* packet = pkt_new();
  pkt_status_code status = pkt_decode(data, len, packet);
  if(status != PKT_OK){
        send_ack(lastackn[connection],lastackt[connection] ,connection, PTYPE_ACK);
        if(log_out){
        printf("Packet number %d invalid: %d\n",packet->SEQNUM,status);}
        got_invalid(connection);
        pkt_del(packet);
        return status;
  }


  return data_req(packet, connection);
}

int openFile(int i){
    char filename[len_format];
    sprintf(filename, format, nb_file);
    int filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 00777);
    if(filefd == -1){fprintf(stderr, "[openFile] : %s\n", strerror(errno)); exit(EXIT_FAILURE);}
    if(log_out)printf("Nouveau file descriptor : %d\n", filefd);
    file_descriptors[i] = filefd;
    nb_file ++;
    return 0;
}

int closeFiles(){
    for (size_t i = 0; i < clients_known; i++) {
            close(file_descriptors[i]);
    }
    return 0;
}
