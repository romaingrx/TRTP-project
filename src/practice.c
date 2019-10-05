#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <zlib.h>
#include <arpa/inet.h>

void test_decode_header();
void test_decode_all();
void test_encode();
void test_pointer_struct();

int maisn(int argc, char const *argv[]) {
  // test_decode_header();
  // test_decode_all();
  test_pointer_struct();
  return 0;
}


void test_decode_header(){
  pkt_t *pkt = pkt_new();
  // uint32_t HEADER = 0b01000011000001110000001100000000; // TYPE = 1, TR = 0, WINDOW = 3, L = 0, LENGTH = 7, SEQNUM = 3
  uint32_t HEADER = 0b01000011100000000000011100000011; // TYPE = 1, TR = 0, WINDOW = 3, L = 1, LENGTH = 7, SEQNUM = 3
  HEADER = htonl(HEADER);
  char * DATA = (char*) &HEADER;
  pkt_t *pktd = pkt_new();
  pkt_decode(DATA, 4, pktd);
  printf("TYPE \t%u\n", pkt_get_type(pktd));
  printf("TR \t%u\n", pkt_get_tr(pktd));
  printf("WINDOW \t%u\n", pkt_get_window(pktd));
  printf("LENGTH \t%u\n", pkt_get_length(pktd));
  printf("SEQNUM \t%u\n", pkt_get_seqnum(pktd));
  pkt_del(pkt);
  pkt_del(pktd);
}

void test_decode_all(){
  pkt_t *pkt = pkt_new();
  pkt_t *pktd = pkt_new();

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

  uint32_t HEADER;
  memcpy(&HEADER, (void*)pkt, 4);
  // HEADER = htonl(HEADER);
  uint32_t FILTER = 0b00000000000000000000000011111111;
  printf("%u\n", HEADER&FILTER);
  char * DATA = (char*) &HEADER;
  pkt_decode(DATA, 4, pktd);

  printf("TYPE \t%u\n", pkt_get_type(pktd));
  printf("TR \t%u\n", pkt_get_tr(pktd));
  printf("WINDOW \t%u\n", pkt_get_window(pktd));
  printf("LENGTH \t%u\n", pkt_get_length(pktd));
  printf("SEQNUM \t%u\n", pkt_get_seqnum(pktd));
  // printf("TIMESTAMP \t%u\n", pkt_get_timestamp(pktd));
  // printf("CRC1 \t%u\n", pkt_get_crc1(pktd));
  // printf("PAYLOAD \t%s\n", pkt_get_payload(pktd));
  // printf("CRC2 \t%u\n", pkt_get_crc2(pktd));

  pkt_del(pkt);
  pkt_del(pktd);
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

  // printf("TYPE \t%p\n", &(pkt->TYPE));
  // printf("TR \t%p\n", &(pkt->TR));
  // printf("WINDOW \t%p\n", &(pkt->WINDOW));
  // printf("LENGTH \t%p\n", &(pkt->LENGTH));
  // printf("SEQNUM \t%p\n", &(pkt->SEQNUM));
  // printf("TIMESTAMP \t%u\n", pkt_get_timestamp(pktd));
  // printf("CRC1 \t%u\n", pkt_get_crc1(pktd));
  // printf("PAYLOAD \t%s\n", pkt_get_payload(pktd));
  // printf("CRC2 \t%u\n", pkt_get_crc2(pktd));

}

void test_encode(){

}

typedef struct st st_t;

struct __attribute__((__packed__)) st {
    unsigned int SEQNUM : 8;
    unsigned int LENGTH : 15;
    unsigned int L : 1;
    unsigned int WINDOW : 5;
    unsigned int TR : 1;
    unsigned int TYPE : 2;

};

int main(int argc, char const *argv[]) {
  st_t *st = (st_t*) malloc(sizeof(st_t));
  st->TYPE = 1;
  st->TR = 0;
  st->WINDOW = 16;
  uint8_t *u = malloc(sizeof(uint8_t));
  memcpy(u, st, 1);
  printf("%u\n",*u);
  return 0;
}
