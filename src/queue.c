#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//ONLY FOR TESTING
typedef struct pkt{
  int WINDOW;
  int SEQNUM;
  int valid_packet;
}pkt_t;
//Temporary package structure testing interface
int windowsize = 4;
int n_bits_encode_window = 16;
//END OF ONLY FOR TESTING


int lastack = -1;//Last packet for which an ack was send
int next = 0;//Expected next package

int window_start = 0;//Window starting number
int window_end = 0;//Window ending number set to windowsize-1;


//BUFFER IMPLEMENTATION
typedef struct node {
    pkt_t *data;
    struct node * next;
} node_t;

node_t *head = NULL;
node_t *last = NULL;
/*
* This function adds a packet to the buffer, at the end of it
*/
void buffer_add(pkt_t *pkt){
  node_t *newnode = malloc(sizeof(node_t));
  if(newnode == NULL){
    printf("newnode malloc failed");
    exit(-1);
  }
  newnode->data = pkt;
  newnode->next = NULL;
  if(head == NULL){
    head = newnode;
  }
  else{
    node_t *runner = last;
    runner->next = newnode;
  }
  last = newnode;
  printf("ADDED %d to buffer\n", newnode->data->SEQNUM);

}
/*
*This function returns the head of the buffer
*/
pkt_t* buffer_peak(){
  if(head == NULL){
    return NULL;
  }
  pkt_t* ret = head->data;
  return ret;
}
/*
* This function removes the head of the buffer and replaces it with the next node;
*/
void buffer_remove(){
  node_t* oldhead = head;
  head = head->next;//no nullpointer since head->next = NULL on the last one
  free((void*) oldhead);
}


//BUFFER IMPLEMENTATION END/

//This function takes a bytestream and decodes it. It returns 0 if the packet is invalide
//and 1 if it is valid; --> for testing purpuses I am inputting a packet.
int decode_pkt(pkt_t *pkt){
  return pkt->valid_packet;
}

//This function sends a packet to the final destination (the txt).
//For now it only prints a packet was recieved
void data_ind(pkt_t *pkt){
  printf("Successfully recieved data %d\n", pkt->SEQNUM);
  free(pkt);
}

//This function increases the "next" variable
void next_inc(){
  if(next < pow(2,n_bits_encode_window)){
    next++;
    return;
  }

  next = 0;
}
void window_inc(){
  if(window_start < pow(2,n_bits_encode_window)){  window_start ++;}
  else {window_start = 0;}
  if(window_end < pow(2,n_bits_encode_window)){  window_end ++;}
  else {window_end = 0;}
  printf("[%d,%d]\n", window_start, window_end);
}

//This function sends a PTYPE_ACK to the sender
void send_ack(int n){
  printf("ACK %d\n", n);
  lastack = n;
  if(window_start == n){
    window_inc();
  }
}


//This function takes a bytestream and treats it. Always returns 0;
//For testing it currently takes in a packet.
int data_req(pkt_t *pkt){
  //This function should recieve a bytestream;
  //First we should send decode it.
  if(!decode_pkt(pkt)){
    //If packet not valid;
    printf("Packet invalid\n");
    send_ack(lastack);
    return 0;
  }
  int n = pkt->SEQNUM;
  if(window_start < window_end){
    if(n < window_start || n > window_end){
      //Packet not inside window, ignore it
      printf("Out of window packet 1\n");
      return 0;
    }
  }
  if(window_start > window_end){
    if(pkt->SEQNUM > window_start || pkt->SEQNUM < window_end){
      //Packet not inside window, ignore it
      printf("Out of window packet 2\n");
      return 0;
    }
  }






  if(n == next){
    //If the packet is in sequence
    next_inc();
    data_ind(pkt) ;
    send_ack(n);

    //I recieved a valid packet, now I check in the buffer if I had stored the next Expected
    //packet. If it is in the buffer I call this function recursively with it. Otherwise the function ends.
    //do that by calling data_req(buffer.next) (recursion)
    pkt_t* buf = buffer_peak();
    if(buf != NULL){
      if(buf->SEQNUM == next){
        buffer_remove();
        data_req(buf);
      }
    }

    return 0;
  }
  else{
    //if the packet is out of sequence BUT inside the recieving window:
    //add it to buffer
    if(head != NULL){
      if(buffer_peak()->SEQNUM == n){
        printf("Already in buffer\n");
        return 0;
      }
    }
    buffer_add(pkt);
    send_ack(lastack);
  }
}





//TOUT EN DESSOUS ICI EST JUSTE POUR FAIRE DES TESTS
pkt_t* pkt_new(int seq, int valid)
{
    pkt_t *pkt = (pkt_t*) malloc(sizeof(pkt_t));
    if (pkt == NULL) {
      perror("Erreur lors du malloc du package");
      return NULL;
    }
    pkt->WINDOW = windowsize;
    pkt->SEQNUM = seq;
    pkt->valid_packet = valid;
    return pkt;
}



int main(int argc, char const *argv[]) {
  window_start=0;
  window_end=windowsize-1;

  pkt_t *pkt0 = pkt_new(0, 1);
  pkt_t *pkt1 = pkt_new(1, 1);
  pkt_t *pkt2 = pkt_new(2, 1);
  pkt_t *pkt3 = pkt_new(3, 1);
  pkt_t *pkt4 = pkt_new(4, 1);
  pkt_t *pkt5 = pkt_new(0, 1);
  pkt_t *pkt6 = pkt_new(1, 1);
  pkt_t *pkt7 = pkt_new(2, 1);
  pkt_t *pkt8 = pkt_new(3, 1);
  pkt_t *pkt9 = pkt_new(4, 1);
  pkt_t *pkt10 = pkt_new(0,1);

  data_req(pkt0);
  data_req(pkt1);
  data_req(pkt3);
  data_req(pkt3);
  data_req(pkt2);

  //data_req(pkt1);
  //data_req(pkt5);
  /*data_req(pkt4);
  data_req(pkt5);
  data_req(pkt6);
  data_req(pkt7);
  data_req(pkt8);
  data_req(pkt9);
  data_req(pkt10);*/


  return 0;

}


//End of package structure testing interface
