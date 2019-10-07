#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <errno.h>

/* Extra #includes */
/* Your code will be inserted here */

/* Package structure */
// struct __attribute__((__packed__)) pkt {
//     unsigned int TYPE : 2;
//     unsigned int TR : 1;
//     unsigned int WINDOW : 5;
//     uint8_t SEQNUM;
//     unsigned int L : 1;
//     uint16_t LENGTH;
//     uint32_t TIMESTAMP;
//     uint32_t CRC1;
//     char * PAYLOAD;
//     uint32_t CRC2;
// };

pkt_t* pkt_new()
{
    pkt_t *pkt = (pkt_t*) malloc(sizeof(pkt_t));
    if (pkt == NULL) {
      fprintf(stderr, "Erreur lors du malloc du package");
      return NULL;
    }
    // pkt->TYPE = 1;
    // pkt->TR = 0;
    // pkt->WINDOW = 0;
    // pkt->LENGTH = 0; // 0 == htons(0)
    // pkt->TIMESTAMP = 0;
    // pkt->CRC1 = 0;
    pkt->PAYLOAD = NULL;
    return pkt;
}

void pkt_del(pkt_t *pkt)
{
    if(pkt != NULL){  //Vérifie que le packet est contenu en mémoire avant de le libérer
      if(pkt->PAYLOAD != NULL){ //Vérifie que la PAYLOAD est contenue en mémoire avant de la libérer
          free(pkt->PAYLOAD);
      }
      free(pkt);
    }
}

pkt_status_code header_decode(pkt_t *pkt,const char *buf, size_t *offset){

    char first_byte = buf[0];
    pkt_status_code status;
    status = pkt_set_type(pkt, binary_decode_type(first_byte));
    if(status != PKT_OK){return status;}
    status = pkt_set_tr(pkt, binary_decode_tr(first_byte));
    if(status != PKT_OK){return status;}
    status = pkt_set_window(pkt, binary_decode_window(first_byte));
    if(status != PKT_OK){return status;}

    uint8_t *data = malloc(2);
    uint16_t retval;
    memcpy(data, (void*)&buf[1],2);
    size_t L = varuint_decode(data, 2, &retval);
    free(data);
    if(L != 0 && L != 1){return E_OTHER;}
    status = pkt_set_l(pkt, L);
    if(status != PKT_OK){return status;}
    status = pkt_set_length(pkt, retval);
    if(status != PKT_OK){return status;}
    if (L == 0) {*offset = 2;} else {*offset = 3;}

    uint8_t SEQNUM;
    memcpy(&SEQNUM, (void*)&buf[*offset], 1);
    status = pkt_set_seqnum(pkt, SEQNUM);
    if(status != PKT_OK){return status;}
    *offset += 1;
    uint32_t TIMESTAMP;
    memcpy(&TIMESTAMP, (void*)&buf[*offset], 4);
    status = pkt_set_timestamp(pkt, ntohl(TIMESTAMP));
    if(status != PKT_OK){return status;}
    *offset += 4;

    return PKT_OK;
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    pkt_status_code status;
    size_t offset;
    uint32_t CRC1, CRC2;

    /* Vérifie la validité du packet */
    if(!len){return E_UNCONSISTENT;} // 0 bit reçu
    if(len < 4){return E_NOHEADER;} // < 32 bits reçu, header incorrect

    status = header_decode(pkt, data, &offset);
    if(status != PKT_OK){return status;}

    memcpy(&CRC1, (void*)&data[offset], 4);
    status = pkt_set_crc1(pkt, ntohl(CRC1));
    if(status != PKT_OK){return status;}
    offset += 4;

    uint16_t length = pkt_get_length(pkt);
    char *PAYLOAD = malloc(length);
    memcpy(PAYLOAD, (void*)&data[offset], length);
    status = pkt_set_payload(pkt, PAYLOAD, length);
    if(status != PKT_OK){return status;}
    offset += length;

    memcpy(&CRC2, (void*)&data[offset], 4);
    status = pkt_set_crc2(pkt, ntohl(CRC2));
    if(status != PKT_OK){return status;}
    offset += 4;


     free(PAYLOAD);
     return PKT_OK;
}

size_t header_encode(const pkt_t *pkt,char *buf){
    pkt_status_code status;
    uint8_t err, header_length = predict_header_length(pkt);
    memcpy(buf, (void*)pkt, 1); // copy TYPE, TR, WINDOW
    size_t offset;
    if (header_length == 8) {
        offset = 3;
        uint8_t *length_bytes = malloc(2);
        if(varuint_encode(pkt_get_length(pkt), length_bytes, 2) == -1){return 0;}
        memcpy(&buf[1], (void*)&length_bytes[0], 2);
        free(length_bytes);
    } else {
        offset = 2;
        uint8_t second_byte;
        if(varuint_encode(pkt->LENGTH, &second_byte, 1) == -1){return 0;}
        memcpy(&buf[1], &second_byte, 1);
    }
    memcpy(&buf[offset], (void*)&(pkt->SEQNUM), 1);
    uint32_t TIMESTAMP = htonl(pkt_get_timestamp(pkt));
    memcpy(&buf[offset+1], (void*)&TIMESTAMP, 4);
    return (size_t)header_length;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
   pkt_status_code status;
   time_t seconds;
   size_t offset = header_encode(pkt, buf);
   if(offset == 0){return E_OTHER;}
   uint32_t CRC1 = htonl(pkt_get_crc1(pkt));
   memcpy(&buf[offset], &CRC1, 4);
   memcpy(&buf[offset + 4], pkt_get_payload(pkt), pkt_get_length(pkt));
   uint32_t CRC2 = htonl(pkt_get_crc2(pkt));
   memcpy(&buf[offset + 4 + pkt_get_length(pkt)], &CRC2, 4);

   return PKT_OK;
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    return pkt->TYPE;
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    return pkt->TR;
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    return pkt->WINDOW;
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    return pkt->SEQNUM;
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
    if(pkt->TR == 0){return pkt->LENGTH;}
    return 0;
}

uint8_t pkt_get_l(const pkt_t* pkt)
{
    return pkt->L;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    return pkt->TIMESTAMP;
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    return pkt->CRC1;
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
  if(pkt_get_tr(pkt)==0 && pkt_get_length(pkt)!=0){return pkt->CRC2;}
  return 0;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    if(pkt_get_length(pkt)!=0){return pkt->PAYLOAD;}
    return NULL;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
  if(type!=PTYPE_DATA && type!=PTYPE_ACK && type!=PTYPE_NACK){
      fprintf(stderr, "[pkt_set_type] Type (%u) non connu \n", type);
      return E_TYPE;} //Non cohérent dans le type
  pkt->TYPE = type;
  return PKT_OK;
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    if(pkt->TYPE == PTYPE_DATA){
      if(tr>1){return E_TR;}
      pkt->TR = tr;
      return PKT_OK;
    }
    return E_TR;
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    if(window>MAX_WINDOW_SIZE){
        fprintf(stderr, "[pkt_set_window] Window (%u) plus grand que MAX_WINDOW_SIZE (%u) \n", window, MAX_WINDOW_SIZE);
        return E_WINDOW;} //WINDOW trop grand
    pkt->WINDOW = window;
    return PKT_OK;
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    pkt->SEQNUM = seqnum;
    return PKT_OK;
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    if(length>MAX_PAYLOAD_SIZE){
        fprintf(stderr, "[pkt_set_length] Length (%u) plus grand que MAX_PAYLOAD_SIZE (%u) \n", length, MAX_PAYLOAD_SIZE);
        return E_LENGTH;}
    pkt->LENGTH = length;
    return PKT_OK;
}

pkt_status_code pkt_set_l (pkt_t* pkt, const uint8_t l){
    if (l > 1) {return E_LENGTH;}
    pkt->L = l;
    return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    pkt->TIMESTAMP = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    pkt->CRC1 = crc1;
    return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
  pkt->CRC2 = crc2;
  return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    pkt_status_code status = pkt_set_length(pkt, length); //pkt_get_length(pkt) == length
    if(pkt_get_payload(pkt) != NULL){free(pkt->PAYLOAD);}
    if(status != PKT_OK){return status;}
    pkt->PAYLOAD = (char*) malloc(length);
    memcpy(pkt->PAYLOAD, data, length);
    return PKT_OK;
}

ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)
{
    if(len != 1 && len != 2){
        fprintf(stderr, "[varuint_decode] Dimension %zu différente de {1, 2} byte(s)\n", len);
        return -1;}
    uint8_t L = varuint_len(data);
    if(L == 0){
        *retval = (uint16_t)data[0];
    }else if(L == 1){
        uint16_t NBO= (uint16_t)((data[0] & 0b01111111) << 8); // Les 7 premiers bits
        NBO += (uint8_t)(data[1]); // Les 8 bits suivants
        *retval = ntohs(NBO);
    }
    return L;
}

ssize_t varuint_decode_experimental(const uint8_t *data, const size_t len, uint16_t *retval)
{
    if(len != 1 && len != 2){
        fprintf(stderr, "[varuint_decode] Dimension %zu différente de {1, 2} byte(s)\n", len);
        return -1;}
    uint8_t L = varuint_len(data);
    if(L == 1){
        *retval = (uint16_t)data[0];
    }else if(L == 2){
            uint16_t NBO;
        if (ntohs(1) == 1){
            NBO += (uint8_t)(data[1]); // Les 8 bits suivants
            *retval = ntohs(NBO);
        } else {
            NBO = (uint16_t)(data[0] & 0b01111111);
        }
    }
    return L;
}

ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)
{
    if(len != 1 && len != 2){
        fprintf(stderr, "[varuint_encode] Dimension %zu différente de {1, 2} byte(s)\n", len);
        return -1;} // len != {1, 2} byte(s)
    if(val > MAX_15BITS){
        fprintf(stderr, "[varuint_encode] Valeur %u trop grande pour être encodée sur 15 bits (L=1)\n", val);
        return -1;} // Valeur trop grande pour etre encodee sur 15 bits
    uint8_t L = varuint_predict_len(val);
    if(len < (L+1)){
        fprintf(stderr, "[varuint_encode] Dimension %zu de data trop petite pour stocker %u bytes \n", len, L);
        return -1;} // Dimension de data trop petite pour stocker val
    if(L == 0){
        data[0] = (uint8_t)val;
    }else if(L == 1){
        val = htons(val);
        data[0]  = (uint8_t)((val >> 8) & 0b01111111);
        data[0] += (uint8_t)pow(2,7); // Ajoute la valeur de L
        data[1]  = (uint8_t)val;
    }
    return L;
}



ssize_t varuint_encode_experimental(uint16_t val, uint8_t *data, const size_t len)
{
    if(len != 1 && len != 2){
        fprintf(stderr, "[varuint_encode] Dimension %zu différente de {1, 2} byte(s)\n", len);
        return -1;} // len != {1, 2} byte(s)
    if(val > MAX_15BITS){
        fprintf(stderr, "[varuint_encode] Valeur %u trop grande pour être encodée sur 15 bits (L=1)\n", val);
        return -1;} // Valeur trop grande pour etre encodee sur 15 bits
    uint8_t L = len;
    if(len < (L+1)){
        fprintf(stderr, "[varuint_encode] Dimension %zu de data trop petite pour stocker %u bytes \n", len, L);
        return -1;} // Dimension de data trop petite pour stocker val
    if(L == 0){
        data[0] = (uint8_t)val;
    }else if(L == 1){
        if(ntohs(1) == 1){
            data[0]  = (uint8_t)((val >> 8) & 0b01111111);
            data[0] += (uint8_t)pow(2,7); // Ajoute la valeur de L
            data[1]  = (uint8_t)val;
        }else{
            val = htons(val);
            uint8_t first = (uint16_t)(val >> 8);
            uint8_t second = (uint8_t)(val);
            val = second + (first << 7);
            data[0]  = (uint8_t)((val>1) & 0b01111111);
            data[0] += (uint8_t)pow(2,7); // Ajoute la valeur de L
            data[1]  = (uint8_t)(val & 1) << 7;
            data[1] += (uint8_t)(val >> 8);

        }
    }
    return L;
}

size_t varuint_len(const uint8_t *data)
{
    return (data[0]>>7);
}


ssize_t varuint_predict_len(uint16_t val)
{
    if(val >= MAX_15BITS){
        fprintf(stderr, "[varuint_predict_len] Valeur 0x%x plus grande que la valeur 0x%x encodable sur 15 bits\n", val, MAX_15BITS);
        return -1;}
    if(val > (uint8_t)pow(2,7) -1){return (ssize_t)1;} else{return (ssize_t)0;};
}


ssize_t predict_header_length(const pkt_t *pkt)
{
    if(pkt->LENGTH > MAX_15BITS){
        fprintf(stderr, "[predict_header_length] Valeur de pkt->LENGTH 0x%x plus grande que la valeur 0x%x encodable sur 15 bits\n", pkt->LENGTH, MAX_15BITS);
        return -1;}
    return (pkt->L) + 7;
}

uint8_t binary_decode_type(uint8_t first_byte){
  uint8_t decoder = 0b11000000;
  uint8_t good_bits = (decoder & first_byte);
  return good_bits >> 6;
}

uint8_t binary_decode_tr(uint8_t first_byte){
  uint8_t decoder = 0b00100000;
  uint8_t good_bit = decoder & first_byte;
  return good_bit >> 5;
}

uint8_t binary_decode_window(uint8_t first_byte){
  uint8_t decoder = 0b00011111;
  uint8_t good_bits = decoder & first_byte;
  return good_bits >> 0;
}

uint8_t binary_decode_l(uint8_t second_byte){
  uint16_t decoder = 0b10000000;
  uint16_t good_bit = decoder & second_byte;
  return (uint8_t) (good_bit >> 7);
}

uint16_t binary_decode_length(uint8_t L, uint16_t length_bytes){
  uint16_t good_bits;
  if(L == 0){
      uint8_t decoder = 0b01111111;
      uint8_t length_byte = (uint8_t)length_bytes;
      good_bits = decoder & length_bytes;
  }else if(L == 1){
      uint16_t decoder = 0b0111111111111111;
      good_bits = decoder & length_bytes;
  }else{
    uint8_t good_bits = 0;
  }
  return good_bits;
}
