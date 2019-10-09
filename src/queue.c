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
typedef struct node {
    pkt_t* data;
    struct node * next;
} node_t;


int* windowsize = NULL;
int* lastack=NULL;
int* next=NULL;

int* window_start=NULL;
int* window_end=NULL;

node_t** head;


//TOUT EN DESSOUS ICI EST JUSTE POUR FAIRE DES TESTS
pkt_t* pkt_generate(int seq, int valid)
{
  pkt_t packet = pkt_new();
  packet->SEQNUM = seq;
  pkt_set_payload(packet, "Coucou", 7);
  return packet;


}

int decode_pkt(pkt_t *pkt){
  return 1;
}

void free_pkt(pkt_t* pkt){
  // if(pkt->PAYLOAD != NULL){
  //   free(PAYLOAD);
  // }
  free(pkt);
}

void data_ind(pkt_t *pkt, int connection){
  if(log_out){
  printf("Successfully recieved data %d\n", pkt->SEQNUM);}
  free_pkt(pkt);
}
//








int init_queue(int n){
  n_connections  = n;

  int* err;
  err = (int*)malloc(sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  windowsize = err;

  err = (int*)malloc(sizeof(int)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  lastack = err;

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

  for(int i=0; i<n; i++){
    windowsize[i]=4; //Should be changed by program
    lastack[i]=-1;
    next[i]=0;
    window_start[i]=0;
    window_end[i]=0;
  }
  head = (node_t**)malloc(sizeof(node_t*)*n_connections);
  if(head<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }

  return 0;
}



//This function initialises a connection and all its variables
int define_connection(int con_indice, int window_size){
  windowsize[con_indice] = window_size;
  lastack[con_indice]=-1;
  next[con_indice]=0;
  window_start[con_indice]=0;
  window_end[con_indice]=window_size-1;
  head[con_indice]=NULL;
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
      return;
    }
    node_t *runner = head[connection];
    while(runner->next!=NULL && runner->next->data->SEQNUM <n){
      runner = runner->next;
    }
    newnode->next = runner->next;
    runner->next = newnode;
  }
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
    free_pkt(head[connection]->data);
    node_t* oldhead = head[connection];
    head[connection]=oldhead->next;
    free(oldhead);
  }
}

//END OF BUFFER


void free_queue(){
  free(windowsize);
  free(lastack);
  free(next);
  free(window_start);
  free(window_end);

  for(int i =0; i<n_connections;i++){
    free_buffer(i);
  }

  free(head);

}
void next_inc(int connection){
  if(next[connection] < pow(2,n_bits_encode_window)){
    next[connection]++;
    return;
  }
  next = 0;
}

void window_inc(int connection){
  if(window_start[connection] < pow(2,n_bits_encode_window)){  window_start[connection] ++;}
  else {window_start[connection] = 0;}
  if(window_end[connection] < pow(2,n_bits_encode_window)){  window_end[connection] ++;}
  else {window_end[connection] = 0;}
  if(log_out){
  printf("[%d,%d]\n", window_start[connection], window_end[connection]);}
}


void send_ack(int n,int connection){
  if(log_out){
  printf("ACK %d\n", n);}
  lastack[connection] = n;
  if(window_start[connection] == n){
    window_inc(connection);
  }
}




int data_req(pkt_t *pkt, int connection){
  //This function should recieve a bytestream;
  //First we should send decode it.
  if(!decode_pkt(pkt)){
    //If packet not valid;
    if(log_out){
    printf("Packet invalid\n");}
    send_ack(lastack[connection], connection);
    return 0;
  }
  int n = pkt->SEQNUM;
  if(window_start[connection] < window_end[connection]){
    if(n < window_start[connection] || n > window_end[connection]){
      //Packet not inside window, ignore it
      if(log_out){
      printf("Out of window packet 1\n");}
      return 0;
    }
  }
  if(window_start[connection] > window_end[connection]){
    if(pkt->SEQNUM > window_start[connection] || pkt->SEQNUM < window_end[connection]){
      //Packet not inside window, ignore it
      if(log_out){}
      printf("Out of window packet 2\n");
      return 0;
    }
  }
  if(n == next[connection]){
    //If the packet is in sequence
    next_inc(connection);
    data_ind(pkt, connection) ;
    send_ack(n, connection);

    //I recieved a valid packet, now I check in the buffer if I had stored the next Expected
    //packet. If it is in the buffer I call this function recursively with it. Otherwise the function ends.
    //do that by calling data_req(buffer.next) (recursion)
    pkt_t* buf = buffer_peak(connection);
    if(buf != NULL){
      if(buf->SEQNUM == next[connection]){
        buffer_remove(connection);
        data_req(buf, connection);
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
        free_pkt(pkt);
        return 0;
      }
    }
    buffer_add(pkt, connection);
    send_ack(lastack[connection], connection);
  }
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
