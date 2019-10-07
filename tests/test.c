#include "../src/packet_interface.h"
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

int main(int argc, char const *argv[]) {
    char* buf = test_encode();
    test_decode(buf);
    free(buf);
    return 0;
}

void test_varuint_decode_encode(){
    pkt_status_code status ;
    ssize_t err;
    size_t bytes = 2;
    uint16_t retval;
    uint8_t *data_decode = malloc(bytes);
    data_decode[0] = 0b10001110;
    data_decode[1] = 0b100011;
    err  = varuint_decode(data_decode, bytes, &retval);
    uint8_t *data_encode = malloc(bytes);
    err = varuint_encode(retval, data_encode, bytes);
    printf("Data encodé en Network Byte Order avant le décodage :\n");
    printf("      data_decode[0] = %u | data_decode[1] = %u \n",data_decode[0], data_decode[1]);
    printf("Valeur stockée dans le data_decode : %u \n",retval);
    printf("Data encodé en Network Byte Order après l'encodage: \n");
    printf("      data_encode[0] = %u | data_encode[1] = %u \n",data_encode[0], data_encode[1]);
    free(data_decode); free(data_encode);
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
    pkt->CRC1 = 37;
    pkt->CRC2 = 666;
    char* data = "ALLUMER LE FEU";
    pkt_set_payload(pkt, data, pkt->LENGTH);

    size_t len = 128;
    char *buf = malloc(len);
    status = pkt_encode(pkt, buf, &len);
    printf("TYPE : \t%u \n",((uint8_t)buf[0] >> 6));
    printf("TR : \t%u \n",((uint8_t)buf[0] >> 5) & 0b00000001);
    printf("WINDOW : \t%u \n",((uint8_t)buf[0]) & 0b00011111);
    printf("L : \t%u\n", (uint8_t)buf[1]>>7);
    printf("LENGTH : \t%u\n", (uint8_t)buf[1] & 0b01111111);
    char *text = malloc(27);
    memcpy(text, &buf[11], 27);
    printf("DATA : %s\n", text);
    free(text);
    pkt_del(pkt);

    return buf;
}

void test_decode(char* buf){
    pkt_status_code status ;
    pkt_t *pktd = pkt_new();
    status = pkt_decode(buf, 128, pktd);
    printf("Type decode \t: %u\n",pkt_get_type(pktd));
    printf("TR decode \t: %u\n",pkt_get_tr(pktd));
    printf("Window decode \t: %u\n",pkt_get_window(pktd));
    printf("L decode \t: %u\n",pkt_get_l(pktd));
    printf("Length decode \t: %u\n",pkt_get_length(pktd));
    printf("Seqnum decode \t: %u\n",pkt_get_seqnum(pktd));
    printf("Timestamp decode \t: %u\n",pkt_get_timestamp(pktd));
    printf("Crc1 decode \t: %u\n",pkt_get_crc1(pktd));
    printf("Payload decode \t: %s\n",pkt_get_payload(pktd));
    printf("Crc2 decode \t: %u\n",pkt_get_crc2(pktd));
    pkt_del(pktd);
}
