#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int n_bits_encode_window = 5;
int n_connections = -1;
typedef struct node {
    int data;
    struct node * next;
} node_t;


int* windowsize = NULL;
int* lastack=NULL;
int* next=NULL;

int* window_start=NULL;
int* window_end=NULL;

node_t** head=NULL;
node_t**last=NULL;

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
  node_t ** err2;

  err2 = (node_t**)malloc(sizeof(node_t*)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  head = err2;

  err2 = (node_t**)malloc(sizeof(node_t*)*n_connections);
  if(err<0){
    fprintf(stderr,"init_queue malloc erreur");
    return(-1);
  }
  last = err2;

  return 0;
}

void free_queue(){
  free(windowsize);
  free(lastack);
  free(next);
  free(window_start);
  free(window_end);

  free(head);
  free(last);

}



int main(int argc, char const *argv[]) {
  init_queue(3);
  windowsize[0]=1;
  windowsize[1]=3;
  windowsize[2]=4;

  printf("HERE: %d, %d ,%d\n", windowsize[0],windowsize[1],windowsize[2]);
  free_queue();

  return 0;
}
