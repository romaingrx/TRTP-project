#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//ONLY FOR TESTING
typedef struct pkt{
  int WINDOW;
  int SEQNUM;
  int valid_packet;
}pkt_t;
//Temporary package structure testing interface
int windowsize = 4;
int n_bits_encode_window = 32;
//END OF ONLY FOR TESTING


int lastack = -1;
int next = 0;

int window_start = 0;
int window_end = 0;


//This function takes a packet and decodes it. It returns 0 if the packet is invalide
//and 1 if it is valid;
int decode_pkt(pkt_t *pkt){
  return pkt->valid_packet;
}

//This function sends a packet to the final file.
//For now it only prints a packet was recieved
void data_ind(pkt_t *pkt){
  printf("Successfully recieved packet %d\n", pkt->SEQNUM);
  free(pkt);
}

//This function increases the "next" variable
void next_inc(){
  if(next < 2^n_bits_encode_window){
    next++;
    return;
  }

  next = 0;
}
void window_inc(){
  if(window_start < 2^n_bits_encode_window){  window_start ++;}
  else {window_start = 0;}
  if(window_end < 2^n_bits_encode_window){  window_end ++;}
  else {window_end = 0;}
  printf("[%d,%d]\n", window_start, window_end);
}

//This function sends a PTYPE_ACK
void send_ack(int n){
  printf("ACK %d\n", n);
  lastack = n;
  if(window_start == n){
    window_inc();
  }
}


//This function takes a bytestream and treats it. Always returns 0;
int data_req(pkt_t *pkt){
  //This function should recieve a bytestream;
  //First we should send decode it.
  if(!decode_pkt(pkt)){
    //If packet not valid;
    printf("Packet invalid, PTYPE_ACK OK%d\n",lastack);
  }

  if(pkt->SEQNUM < window_start || pkt->SEQNUM > window_end){
    //Packet not inside window, ignore it
    printf("Out of window packet %d\n", pkt->SEQNUM);
    return 0;
  }
  int n = pkt->SEQNUM;
  if(n == next){
    //If the packet is in sequence
    next_inc();
    data_ind(pkt) ;
    send_ack(n);

    //SHOULD NOW SEND ALL PACKETS THAT ARE IN BUFFER AND ACK THEM
    //do that by calling data_req(buffer.next) (recursion)
    return 0;
  }
  else{
    //n not next but is still in the window
    //add it to buffer
    printf("Adding %d to buffer\n",n);
    data_ack(lastack);
  }
}






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

  data_req(pkt0);
  //data_req(pkt1);
  data_req(pkt2);
  return 0;

}


//End of package structure testing interface
