#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <zlib.h>
#include <arpa/inet.h>

void test_header();
void test_timestamp();

int main(int argc, char const *argv[]) {
  test_timestamp();
  return 0;
}


void test_header(){
  pkt_t *pkt = pkt_new();
  // uint32_t HEADER = 0b01000011000001110000011100000000; // TYPE = 1, TR = 0, WINDOW = 3, L = 0, LENGTH = 7, SEQNUM = 3
  uint32_t HEADER = 0b01000011100000000000011100000111; // TYPE = 1, TR = 0, WINDOW = 3, L = 1, LENGTH = 7, SEQNUM = 3
  HEADER = htonl(HEADER);
  char * DATA = (char*) &HEADER;
  pkt_decode(DATA, 4, pkt);
  printf("TYPE \t%u\n", pkt_get_type(pkt));
  printf("TR \t%u\n", pkt_get_tr(pkt));
  printf("WINDOW \t%u\n", pkt_get_window(pkt));
  printf("LENGTH \t%u\n", pkt_get_length(pkt));
  printf("SEQNUM \t%u\n", pkt_get_seqnum(pkt));
  pkt_del(pkt);
}

void test_timestamp(){
  pkt_t *pkt = pkt_new();
  pkt_t *pktd = pkt_new();

  pkt_set_type(pkt, 1);
  pkt_set_tr(pkt, 0);
  pkt_set_window(pkt, 3);
  pkt_set_length(pkt, 1);
  pkt_set_seqnum(pkt, 3);
  pkt_set_timestamp(pkt, 777);
  pkt_set_crc1(pkt, 666);
  char *a = "COUCOUL";
  pkt_set_payload(pkt, a, 8);
  pkt_set_crc2(pkt, 111);

  char *BUF = malloc(13);
  memcpy(BUF, pkt, 13);
  pkt_decode(BUF, 13, pktd);

  printf("TYPE \t%u\n", pkt_get_type(pkt));
  printf("TR \t%u\n", pkt_get_tr(pkt));
  printf("WINDOW \t%u\n", pkt_get_window(pkt));
  printf("LENGTH \t%u\n", pkt_get_length(pkt));
  printf("SEQNUM \t%u\n", pkt_get_seqnum(pkt));
  printf("TIMESTAMP \t%u\n", pkt_get_timestamp(pkt));
  printf("CRC1 \t%u\n", pkt_get_crc1(pkt));
  printf("PAYLOAD \t%s\n", pkt_get_payload(pkt));
  printf("CRC2 \t%u\n", pkt_get_crc2(pkt));

  pkt_del(pkt);
  pkt_del(pktd);
}
