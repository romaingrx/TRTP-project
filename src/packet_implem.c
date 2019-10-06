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

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    pkt_status_code status;
    int byte_start_for_seqnum = 2; // Par défault L = 0
    /* Vérifie la validité du packet */
    if(!len){return E_UNCONSISTENT;} // 0 bit reçu
    if(len < 4){return E_NOHEADER;} // < 32 bits reçu, header incorrect

    /* DECODE LE HEADER */
      uint8_t first_byte = data[0];
      uint8_t second_byte = data[1];
      uint16_t length_bytes;

      /* L */
      uint8_t L = binary_decode_l(second_byte);
      if(L == 1){
        byte_start_for_seqnum = 3;
        memcpy(&length_bytes, &data[1], 2);
      }else{
        memcpy(&length_bytes, &data[1], 1);
      }

      /* TYPE */
      ptypes_t TYPE = binary_decode_type(first_byte);
      if((status = pkt_set_type(pkt, TYPE)) != PKT_OK){return status;}

      /* TR */
      unsigned int TR = binary_decode_tr(first_byte);
      if((status = pkt_set_tr(pkt, TR)) != PKT_OK){return status;}

      /* WINDOW */
      unsigned int WINDOW = binary_decode_window(first_byte);
      if((status = pkt_set_window(pkt, WINDOW)) != PKT_OK){return status;}

      /* SEQNUM */
      uint8_t SEQNUM = data[byte_start_for_seqnum];
      if((status = pkt_set_seqnum(pkt, SEQNUM)) != PKT_OK){return status;}

      /* LENGTH */
      if(L==1){length_bytes = ntohs(length_bytes);} // Pas dans le cas où L == 0 car uint8_t encodé sur un uint16_t donc ça change sa vraie valeure
      uint16_t LENGTH = binary_decode_length(L, length_bytes);
    	status = pkt_set_length(pkt, LENGTH);
    	if(status != PKT_OK){return status;}

      /* TIMESTAMP */
      uint32_t TIMESTAMP = *((uint32_t *)(data + (byte_start_for_seqnum + 1)));
      if((status = pkt_set_timestamp(pkt, TIMESTAMP)) != PKT_OK){return status;}

      /* CRC1 */
      uint32_t CRC1 = (*((uint32_t *)(data + (byte_start_for_seqnum + 5))));
      status = pkt_set_crc1(pkt, CRC1);
      if(status != PKT_OK){return status;}

      /* PAYLOAD */
      char data_payload[LENGTH];
      memcpy(&data_payload, ((void *)(data + (byte_start_for_seqnum + 9))), LENGTH);
      status = pkt_set_payload(pkt, (const char *)&data_payload, LENGTH);
      if(status != PKT_OK){return status;}

      /* CRC2 */
      uint32_t CRC2 = (*((uint32_t *)(data + (byte_start_for_seqnum + LENGTH + 9))));
      status = pkt_set_crc2(pkt, CRC2);
      if(status != PKT_OK){return status;}



      return PKT_OK;
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
   pkt_status_code status;
   time_t seconds;
   printf("%s\n",buf);
   printf("%u\n",pkt->LENGTH);
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
    if(pkt->TR == 0){return ntohs(pkt->LENGTH);}
    return 0;
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    return ntohl(pkt->TIMESTAMP);
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    return ntohl(pkt->CRC1);
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
  if(pkt_get_tr(pkt)==0 && pkt_get_length(pkt)!=0){return ntohl(pkt->CRC2);}
  return 0;
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    if(pkt_get_length(pkt)!=0){return pkt->PAYLOAD;}
    return NULL;
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
  if(type!=PTYPE_DATA && type!=PTYPE_ACK && type!=PTYPE_NACK){return E_TYPE;} //Non cohérent dans le type
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
    if(window>MAX_WINDOW_SIZE){return E_WINDOW;} //WINDOW trop grand
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
    if(length>MAX_PAYLOAD_SIZE){return E_LENGTH;}
    pkt->LENGTH = htons(length);
    return PKT_OK;
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    pkt->TIMESTAMP = timestamp;
    return PKT_OK;
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    pkt->CRC1 = htonl(crc1);
    return PKT_OK;
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
  pkt->CRC2 = htonl(crc2);
  return PKT_OK;
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    pkt_status_code pkt_return = pkt_set_length(pkt, length); //pkt_get_length(pkt) == length
    if(pkt_get_payload(pkt) == NULL){free(pkt->PAYLOAD);}
    if(pkt_return != PKT_OK){return pkt_return;}
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
    if(L == 1){
        *retval = (uint16_t)data[0];
    }else if(L == 2){
        uint16_t NBO= (uint16_t)((data[0] & 0b01111111) << 8); // Les 7 premiers bits
        NBO += (uint8_t)(data[1]); // Les 8 bits suivants
        *retval = ntohs(NBO);
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
    if(len < L){
        fprintf(stderr, "[varuint_encode] Dimension %zu de data trop petite pour stocker %u bytes \n", len, L);
        return -1;} // Dimension de data trop petite pour stocker val
    if(L == 1){
        data[0] = (uint8_t)val;
    }else if(L == 2){
        val = htons(val);
        data[0]  = (uint8_t)((val >> 8) & 0b01111111);
        data[0] += (uint8_t)pow(2,7); // Ajoute la valeur de L
        data[1]  = (uint8_t)val;
    }
    return L;
}

size_t varuint_len(const uint8_t *data)
{
    return (data[0]>>7) + 1;
}


ssize_t varuint_predict_len(uint16_t val)
{
    if(val >= MAX_15BITS){
        fprintf(stderr, "[varuint_predict_len] Valeur 0x%x plus grande que la valeur 0x%x encodable sur 15 bits\n", val, MAX_15BITS);
        return -1;}
    if(val > (uint8_t)pow(2,7) -1){return (ssize_t)2;} else{return (ssize_t)1;};
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
