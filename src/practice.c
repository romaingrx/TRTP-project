#include "packet_interface.h"
#include "queue.h"
#include "receive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in6
#include <netdb.h> // addrinfo

void test_decode_header();
void test_decode_all();
void test_encode();
void test_pointer_struct();


int main(int argc, char const *argv[]) {
    struct sockaddr_in6 *ipv6;
    int fd;
    int a = IPV6_translater("192.168.5.1", ipv6);
  return 0;
}


void test_decode_header(){
  pkt_t *pkt = pkt_new();
  // uint32_t HEADER = 0b01000011000001110000001100000000; // TYPE = 1, TR = 0, WINDOW = 3, L = 0, LENGTH = 7, SEQNUM = 3
  uint32_t HEADER = 0b01000011100000000000011100000011; // TYPE = 1, TR = 0, WINDOW = 3, L = 1, LENGTH = 7, SEQNUM = 3
  char *data = malloc(4);
  // data = (char*)&HEADER;
  memcpy(data, &HEADER, 4);
  int err = pkt_decode((char*)&data, 4, pkt);

  pkt_del(pkt);
}

void test_decode_all(){

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
    pkt_t *pkt = pkt_new();
    pkt->TYPE = 0b01;
    pkt->TR = 0;
    pkt->WINDOW = 12;
    pkt->L = 0;
    pkt->LENGTH = 30;
    pkt->SEQNUM = 12;
    pkt->TIMESTAMP = 66;
    pkt->CRC1 = 37;
    pkt->CRC2 = 666;
    char* data = "GROSSE GROSSE BITE DE NOIR";
    pkt_set_payload(pkt, data, pkt->LENGTH);

    size_t len = 128;
    char *buf = malloc(len);
    pkt_status_code status = pkt_encode(pkt, buf, &len);
    printf("TYPE : \t%u \n",((uint8_t)buf[0] >> 6));
    printf("TR : \t%u \n",((uint8_t)buf[0] >> 5) & 0b00000001);
    printf("WINDOW : \t%u \n",((uint8_t)buf[0]) & 0b00011111);
    printf("L : \t%u\n", (uint8_t)buf[1]>>7);
    printf("LENGTH : \t%u\n", (uint8_t)buf[1] & 0b01111111);
    char *text = malloc(27);
    memcpy(text, &buf[11], 27);
    printf("DATA : %s\n", text);

    pkt_t *pktd = pkt_new();
    status = pkt_decode(buf, 128, pktd);
    printf("Type decode \t: %u\n",pkt_get_type(pktd));
    printf("TR decode \t: %u\n",pkt_get_tr(pktd));
    printf("Window decode \t: %u\n",pkt_get_window(pktd));
    printf("L decode \t: %u\n",pkt_get_l(pktd));
    printf("Length decode \t: %u\n",pkt_get_length(pktd));
    printf("Seqnum decode \t: %u\n",pkt_get_seqnum(pktd));
    printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    free(text);
    pkt_del(pkt);
    pkt_del(pktd);
}
