#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "packet.h"
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <unistd.h> // write, close
#include <errno.h>

int log_out = 1; //Makes the program log to stdout

//Global, extern variables:
int master_socket; //The socket for receiving
struct sockaddr_in6* clients; //Saves the IP adresses of known clients
int * file_descriptors = NULL; //Stores the file descriptors for the known clients
int clients_known; //Number of clients CURRENTLY known
int n_bits_encode_window = 8; //should remain constant
int n_connections = -1; //Number of maximal connections (given as argument to main.)

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



//Call this function, by giving it a malloc'd packet and a connection Number
//This function will write the payload to the file descriptor associated to the connection indice
//Frees the packet afterwards
void data_ind(pkt_t *pkt, int connection){

  // if(log_out){
  //   char* payl = pkt_get_payload(pkt);
  // printf("Successfully recieved data%d : %s\n",pkt->SEQNUM, payl);}
  printf("Write in the file \n");
  write(file_descriptors[connection], pkt_get_payload(pkt), pkt_get_length(pkt));
  pkt_del(pkt);
}
//






//This function initialises a connection and all its variables
int define_connection(int con_indice){
  int window_size = 2; //default
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
  n_connections  = n;

  int* err;
  err = (int*)malloc(sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"[init_queue] malloc erreur");
    return(-1);
  }
  windowsize = err;

  lastackn = (uint8_t*)malloc(sizeof(uint8_t)*n_connections);
  if(lastackn==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  lastackt = (uint32_t*)malloc(sizeof(uint32_t)*n_connections);
  if(lastackt==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  err = (int*)malloc(sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  next = err;

  err = (int*)malloc(sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  window_start = err;

  err = (int*)malloc(sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  window_end = err;
  head = (node_t**)malloc(sizeof(node_t*)*n_connections);
  if(head==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  for(int i=0; i<n; i++){
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
  int oldn = n_connections;
  n_connections++;

  int* err;
  err = (int*)realloc(windowsize, sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  windowsize = err;

  lastackn= (uint8_t*)realloc(lastackn, sizeof(uint8_t)*n_connections);
  if(lastackn==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  lastackt= (uint32_t*)realloc(lastackt, sizeof(uint32_t)*n_connections);
  if(lastackt==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  err = (int*)realloc(next, sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  next = err;

  err = (int*)realloc(window_start, sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  window_start = err;

  err = (int*)realloc(window_end, sizeof(int)*n_connections);
  if(err==NULL){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  window_end = err;


  head = (node_t**)realloc(head, sizeof(node_t*)*n_connections);
  if(head==NULL){
    fprintf(stderr,"init_queue malloc erreur");
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

  if(head[connection] != NULL){
    if(head[connection]->data->SEQNUM ==  n+1){
      return;
    }
  }
  //The little if head connection != null makes sure only the last ack is being sent when several
  //packets are stored in the buffer.



  uint8_t toack = 0;
  if(n != 255){toack = n+1;}

  if(log_out){
  printf("ACK %d\n", toack);}

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



  lastackn[connection] = n;
  lastackt[connection] = temps;


  if(window_start[connection] == n){
    window_inc(connection);
  }
}





//This function treats the selective repeat on a packet, on a given connections
//Return value: -1 if error, 0 if all went well.
int data_req(pkt_t* pkt, int connection){
  //If packet was tronqued
  if(pkt->TR==1){
    send_ack(pkt->SEQNUM, pkt->TIMESTAMP,connection, PTYPE_NACK);
  }
  //If variable window size
  if(pkt->WINDOW != windowsize[connection]){
    window_end[connection] = window_start[connection]+pkt->WINDOW -1;
    windowsize[connection] = pkt->WINDOW;
  }


  //Checking if packet is in the receiving window.
  int n = pkt->SEQNUM;
  if(window_start[connection] < window_end[connection]){
    if(n < window_start[connection] || n > window_end[connection]){
      //Packet not inside window, ignore it
      if(log_out){
      printf("Out of window packet 1: %d\n", n);}
      pkt_del(pkt);
      return 0;
    }
  }
  if(window_start[connection] > window_end[connection]){
    if(pkt->SEQNUM > window_start[connection] || pkt->SEQNUM < window_end[connection]){
      //Packet not inside window, ignore it
      if(log_out){}

      printf("Out of window packet 2 %d \n", pkt->SEQNUM);
      pkt_del(pkt);
      return 0;
    }
  }

  if(pkt->TR == 0 && pkt->TYPE == PTYPE_DATA && pkt->LENGTH == 0){
    //If entering here, it means this packet was the very last one of the file.
    //I must
    //1/ End the file descriptor
    //2/ Free this connection's buffer
    //3/ Make sure queue has a free spot for any future connections
    //4/ Clear the known address in the clients list
    clients_known--;
    close(file_descriptors[connection]);
    free_buffer(connection);
    define_connection(connection);
    return 2;

  }


  if(n == next[connection]){
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
        printf("Packet invalid: %d\n",status);}
        return status;
  }


  return data_req(packet, connection);
}



//
// int main(int argc, char const *argv[]) {
//   init_queue(1);
//   define_connection(0, 4);
//
//   pkt_t *pkt0 = pkt_new(0, 1);
//   pkt_t *pkt1 = pkt_new(2, 1);
//   pkt_t *pkt2 = pkt_new(2, 1);
//   data_req(pkt0,0);
//   data_req(pkt1,0);
//   data_req(pkt2,0);
//
//   free_queue();
//
//
//   return 0;
// }
