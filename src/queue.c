#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "packet_interface.h"
int log_out = 1;
// TEMPORARY ZONE
// typedef struct pkt{
//   int WINDOW;
//   int SEQNUM;
//   int valid_packet;
// }pkt_t;

// END OF TEMPORARY ZONE

int n_bits_encode_window = 5;
int n_connections = -1;

int get_nconnections(){
  return n_connections;
}
typedef struct node {
    pkt_t* data;
    struct node * next;
} node_t;


int* windowsize = NULL;
uint8_t* lastackn=NULL;
uint32_t* lastackt = NULL;
int* next=NULL;

int* window_start=NULL;
int* window_end=NULL;

node_t** head;


pkt_t* pkt_generate(int seq, int wid)
{
  pkt_t* packet = pkt_new();
  packet->SEQNUM = seq;
  packet->WINDOW = wid;
  return packet;


}

int decode_pkt(pkt_t *pkt){
  return 1;
}


void data_ind(pkt_t *pkt, int connection){

  if(log_out){
  printf("Successfully recieved data %d: %s\n", pkt->SEQNUM, pkt->PAYLOAD);}
  pkt_del(pkt);
  // free(pkt->PAYLOAD);
  // free(pkt);
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


int init_queue(int n){
  n_connections  = n;

  int* err;
  err = (int*)malloc(sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  windowsize = err;

  lastackn = (uint8_t*)malloc(sizeof(uint8_t)*n_connections);
  if(lastackn<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  lastackt = (uint32_t*)malloc(sizeof(uint32_t)*n_connections);
  if(lastackt<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  err = (int*)malloc(sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  next = err;

  err = (int*)malloc(sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  window_start = err;

  err = (int*)malloc(sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  window_end = err;
  head = (node_t**)malloc(sizeof(node_t*)*n_connections);
  if(head<0){
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
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  windowsize = err;

  lastackn= (uint8_t*)realloc(lastackn, sizeof(uint8_t)*n_connections);
  if(lastackn<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  lastackt= (uint32_t*)realloc(lastackt, sizeof(uint32_t)*n_connections);
  if(lastackt<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  err = (int*)realloc(next, sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  next = err;

  err = (int*)realloc(window_start, sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  window_start = err;

  err = (int*)realloc(window_end, sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  window_end = err;


  head = (node_t**)realloc(head, sizeof(node_t*)*n_connections);
  if(head<0){
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
pkt_t* buffer_peak(int connection){
  if(head[connection]==NULL){
    return NULL;
  }
  pkt_t* ret = head[connection]->data;
  return ret;
}

void buffer_remove(int connection){
  node_t* oldhead = head[connection];
  head[connection] = head[connection]->next;
  free((void*)oldhead);
}

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
  next[connection]++;
  return;
}

void window_inc(int connection){
  if(window_start[connection] < pow(2,n_bits_encode_window)){  window_start[connection] ++;}
  else {window_start[connection] = 0;}
  if(window_end[connection] < pow(2,n_bits_encode_window)){  window_end[connection] ++;}
  else {window_end[connection] = 0;}
  if(log_out){
  printf("[%d,%d]\n", window_start[connection], window_end[connection]);}
}


void send_ack(uint8_t n, uint32_t time,int connection){

  //Pour pas envoyer le mauvais ack
  if(head[connection] != NULL){
    if(head[connection]->data->SEQNUM ==  n+1){
      return;
    }
  }
  uint8_t toack = 0;
  if(n != 255){
    toack = n+1;
  }

  if(log_out){
  printf("ACK %d\n", toack);
}

  //HERE GENERATE A PACKET AND SEND IT via sockets
  //With SEQNUM = n+1
  //TIMESTAMP = TIME

  lastackn[connection] = n;
  lastackt[connection] = time;


  if(window_start[connection] == n){
    window_inc(connection);
  }
}






int data_req(pkt_t* pkt, int connection){


  //TODO (rapide):
  if(pkt->WINDOW != windowsize[connection]){
    window_end[connection] = window_start[connection]+pkt->WINDOW -1;
    windowsize[connection] = pkt->WINDOW;
  }


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

      printf("Out of window packet 2\n");
      pkt_del(pkt);
      return 0;
    }
  }
  if(n == next[connection]){
    //If the packet is in sequence
    next_inc(connection);
    send_ack(pkt->SEQNUM, pkt->TIMESTAMP, connection);
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
    send_ack(lastackn[connection],lastackt[connection], connection);
    return 0;
  }
}




pkt_status_code treat_bytestream(char* data, size_t len, int connection){
  pkt_t* packet = pkt_new();
  pkt_status_code status = pkt_decode(data, len, packet);
  if(status != PKT_OK){
        send_ack(lastackn[connection],lastackt[connection] ,connection);
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
