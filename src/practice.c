#include "packet.h"
#include "queue.h"
#include "receive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo
#include <unistd.h>
#include <fcntl.h>
#define LEN 5


void test_decode_header();
void test_decode_all();
void test_encode();
void test_pointer_struct();
void write_packet(int seqnum, char * payload);

int * tab = NULL;
int clients_known = 0;

int rearange(int connection){
    if (clients_known>1) {
        for (size_t i = connection; i < clients_known-1; i++) {
            tab[i] = tab[i+1];
        }
        if(realloc(tab, clients_known-1) == NULL){
            return -1;
        }
    } else {
        free(tab);
        tab = malloc(sizeof(int)*1);
    }
    clients_known--;
    return 0;
}

int main(int argc, char const *argv[]) {
  //socket_listening(NULL, 8555, 1, "Coucou %d.txt");
  tab = malloc(sizeof(int)*LEN);
  for (size_t i = 0; i < LEN; i++) {
      tab[i] = i;
      clients_known ++;
  }
  for (size_t i = 0; i < clients_known; i++) {
      printf("%d ", tab[i]);
  }
  printf("\n");

  return 0;
}

void test_decode_header(){
  pkt_t *pkt = pkt_new();
  // uint32_t HEADER = 0b01000011000001110000001100000000; // TYPE = 1, TR = 0, WINDOW = 3, L = 0, LENGTH = 7, SEQNUM = 3
  uint32_t HEADER = 0b01000011100000000000011100000011; // TYPE = 1, TR = 0, WINDOW = 3, L = 1, LENGTH = 7, SEQNUM = 3
  char *data = malloc(4);
  // data = (char*)&HEADER;
  memcpy(data, &HEADER, 4);
  pkt_decode((char*)&data, 4, pkt);

  pkt_del(pkt);
}


void test_pointer_struct(){
  pkt_t *pkt = pkt_new();

  pkt_set_type(pkt, 1);
  pkt_set_tr(pkt, 0);
  pkt_set_window(pkt, 3);
  pkt->L = 1;
  pkt_set_length(pkt, 1);
  pkt_set_seqnum(pkt, 3);
  pkt_set_timestamp(pkt, 777);
  pkt_set_crc1(pkt, 666);
  char *a = "COUCOUL";
  pkt_set_payload(pkt, a, 8);
  pkt_set_crc2(pkt, 111);

}




void write_packet(int seqnum, char * payload){
    pkt_t *pkt = pkt_new();
    pkt->TYPE = 0b01;
    pkt->TR = 0;
    pkt->WINDOW = 12;
    pkt->SEQNUM = seqnum;
    pkt_set_payload(pkt, payload, strlen(payload));

    size_t len = 512;
    char *buf = malloc(len);
    printf("BEFORE LEN \t %zu \n", len);
    pkt_encode(pkt, buf, &len);
    printf("AFTER LEN \t %zu \n", len);
    char filename[30];
    char * format = "packets/packet%d";
    snprintf(filename, strlen(format)+4, format, seqnum);
    int filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC);
    write(filefd, buf, len);
    close(filefd);
    free(buf);
}
