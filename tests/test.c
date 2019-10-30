#include "../src/packet.h"
#include "../src/queue.h"
#include "../src/receive.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <zlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <errno.h>


void test_varuint_decode_encode();
char* test_encode();
void test_decode(char* buf);
void test_queue();
void testdeben();


int main(int argc, char const *argv[]) {
    char* buf = test_encode();
    test_decode(buf);
    test_varuint_decode_encode();
    testdeben();
    // test_queue();
    return 0;
}

void test_varuint_decode_encode(){
    ssize_t err;
    size_t bytes = 2;
    uint16_t retval;
    uint8_t *data_decode = malloc(bytes);
    data_decode[0] = 0b10001110;
    data_decode[1] = 0b100011;
    err  = varuint_decode(data_decode, bytes, &retval);
    if(err<0)exit(-1);
    uint8_t *data_encode = malloc(bytes);
    err = varuint_encode(retval, data_encode, bytes);
    if(err<0)exit(-1);
    printf("Data encodé en Network Byte Order avant le décodage :\n");
    printf("      data_decode[0] = %u | data_decode[1] = %u \n",data_decode[0], data_decode[1]);
    printf("Valeur stockée dans le data_decode : %u \n",retval);
    printf("Data encodé en Network Byte Order après l'encodage: \n");
    printf("      data_encode[0] = %u | data_encode[1] = %u \n",data_encode[0], data_encode[1]);
    free(data_decode); free(data_encode);
}

void testdeben(){
  printf("\n\n ============================\n\n TEST DU BUFFER \n\n");
  // pkt_status_code status ;
  pkt_t *pkt = pkt_new();
  pkt->TYPE = 0b01;
  pkt->TR = 0;
  pkt->WINDOW = 12;
  pkt->L = 0;
  pkt->LENGTH = 30;
  pkt->SEQNUM = 3;
  pkt->TIMESTAMP = 66;
  // char* data = "ALLUMER LE FEU";
  // pkt_set_payload(pkt, data, pkt->LENGTH);

  init_queue(1);
  printf("Adding packet number %d to the buffer.\n", pkt->SEQNUM);
  buffer_add(pkt, 0);

  // pkt_status_code status ;
  pkt_t *pkt2 = pkt_new();
  pkt2->TYPE = 0b01;
  pkt2->TR = 0;
  pkt2->WINDOW = 12;
  pkt2->L = 0;
  pkt2->LENGTH = 30;
  pkt2->SEQNUM = 2;
  pkt2->TIMESTAMP = 66;
  // char* data = "ALLUMER LE FEU";
  // pkt_set_payload(pkt2, data, pkt->LENGTH);
  printf("Adding packet number %d to the buffer.\n", pkt2->SEQNUM);
  buffer_add(pkt2, 0);

  printf("First packet on buffer: %d\n", buffer_peak(0)->SEQNUM);
  buffer_remove(0);
  printf("First packet on buffer: %d\n\n\n\n", buffer_peak(0)->SEQNUM);
  buffer_remove(0);
  free_queue();
  free(pkt);
  free(pkt2);
}

char* test_encode(){
    pkt_status_code status ;
    pkt_t *pkt = pkt_new();
    pkt->TYPE = 0b01;
    pkt->TR = 0;
    pkt->WINDOW = 12;
    pkt->L = 0;
    pkt->LENGTH = 30;
    pkt->SEQNUM = 12;
    pkt->TIMESTAMP = 66;
    char* data = "ALLUMER LE FEU";
    pkt_set_payload(pkt, data, pkt->LENGTH);

    size_t len = 128;
    char *buf = malloc(len);
    status = pkt_encode(pkt, buf, &len);
    if(status<0)exit(-1);
    printf("-------------------------------------\n");
    printf("------------- Encodage --------------\n");
    printf("-------------------------------------\n");
    printf("TYPE : \t\t%u \n",((uint8_t)buf[0] >> 6));
    printf("TR : \t\t%u \n",((uint8_t)buf[0] >> 5) & 0b00000001);
    printf("WINDOW : \t%u \n",((uint8_t)buf[0]) & 0b00011111);
    printf("L : \t\t%u\n", (uint8_t)buf[1]>>7);
    printf("LENGTH : \t%u\n", (uint8_t)buf[1] & 0b01111111);
    char *text = malloc(27);
    memcpy(text, &buf[11], 27);
    printf("DATA : \t\t%s\n\n", text);
    free(buf);
    free(text);

    return buf;
}

void test_decode(char* buf){
    pkt_status_code status ;
    pkt_t *pktd = pkt_new();
    status = pkt_decode(buf, 128, pktd);
    if(status<0)exit(-1);
    printf("-------------------------------------\n");
    printf("------------- Decodage --------------\n");
    printf("-------------------------------------\n");
    printf("TYPE  : \t%u\n",pkt_get_type(pktd));
    printf("TR : \t\t%u\n",pkt_get_tr(pktd));
    printf("WINDOW : \t%u\n",pkt_get_window(pktd));
    printf("L : \t\t%u\n",pkt_get_l(pktd));
    printf("Length : \t%u\n",pkt_get_length(pktd));
    // printf("Seqnum decode \t: %u\n",pkt_get_seqnum(pktd));
    // printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    // printf("Crc1 decode \t: %u\n",pkt_get_crc1(pktd));
    printf("Payload : \t%s\n\n",pkt_get_payload(pktd));
    // printf("Crc2 decode \t: %u\n",pkt_get_crc2(pktd));
    pkt_del(pktd);
}


void test_queue(){
  init_queue(1);
  for(int i = 10; i>=0; i--){
  pkt_t *pkt = pkt_new();
  pkt->TYPE = 0b01;
  pkt->TR = 0;
  pkt->WINDOW = 17;
  pkt->L = 0;
  pkt->LENGTH = 30;
  pkt->SEQNUM = i;
  pkt->TIMESTAMP = 66;
  // pkt->CRC1 = 37;
  // pkt->CRC2 = 666;
  char* data = "GROSSE GROSSE BITE DE NOIR";
  pkt_set_payload(pkt, data, strlen(data)+1);



  char* donnees = malloc(sizeof(char)*1024);
  size_t coucou = 1024;
  pkt_encode(pkt, donnees,&coucou);

  pkt_t* test2 = pkt_new();
  pkt_decode(donnees, coucou,test2 );
  treat_bytestream(donnees, coucou, 0);
  // data_req(pkt,0);
}
add_queue();
printf("-------------------------\n");
for(int i = 0; i<10; i++){
pkt_t *pkt = pkt_new();
pkt->TYPE = 0b01;
pkt->TR = 0;
pkt->WINDOW = 17;
pkt->L = 0;
pkt->LENGTH = 30;
pkt->SEQNUM = i;
pkt->TIMESTAMP = 66;
// pkt->CRC1 = 37;
// pkt->CRC2 = 666;
char* data = "GROSSE GROSSE BITE DE NOIR";
pkt_set_payload(pkt, data, strlen(data)+1);



char* donnees = malloc(sizeof(char)*1024);
size_t coucou = 1024;
pkt_encode(pkt, donnees,&coucou);

pkt_t* test2 = pkt_new();
pkt_decode(donnees, coucou,test2 );
treat_bytestream(donnees, coucou, 1);
// data_req(pkt,0);
}
}
