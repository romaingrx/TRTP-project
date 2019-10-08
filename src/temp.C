#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int log_out = 1;
// TEMPORARY ZONE
typedef struct pkt{
  int WINDOW;
  int SEQNUM;
  int valid_packet;
}pkt_t;

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
pkt_t* pkt_new(int seq, int valid)
{
    pkt_t *pkt = (pkt_t*) malloc(sizeof(pkt_t));
    if (pkt == NULL) {
      perror("Erreur lors du malloc du package");
      return NULL;
    }
    pkt->WINDOW = windowsize[0];
    pkt->SEQNUM = seq;
    pkt->valid_packet = valid;
    return pkt;
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

void free_queue(){
  free(windowsize);
  free(lastack);
  free(next);
  free(window_start);
  free(window_end);

  free(head);

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



//END OF BUFFER IMPLEMENTATION
int main(int argc, char const *argv[]) {
  init_queue(1);
  define_connection(0, 4);

  pkt_t *pkt0 = pkt_new(0, 1);
  pkt_t *pkt1 = pkt_new(4, 1);


  buffer_add(pkt0,0);
  buffer_add(pkt1,0);

  free(pkt0);
  free(pkt1);
  //Ici j'ai pas free les node donc j'ai 3 alloc?

  free_queue();


  return 0;
}
