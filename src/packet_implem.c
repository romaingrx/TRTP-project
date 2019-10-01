#include "packet_interface.h"

/* Extra #includes */
/* Your code will be inserted here */

/* Package structure */
struct __attribute__((__packed__)) pkt {
    unsigned int TYPE : 2;
    unsigned int TR : 1;
    unsigned int WINDOW : 5;
    unsigned int L : 1;
    unsigned int LENGTH;
    uint8_t SEQNUM;
    uint32_t TIMESTAMP;
    uint32_t CRC1;
    char * PAYLOAD;
    uint32_t CRC2;
};

pkt_t* pkt_new()
{
    pkt_t *pkt = (pkt_t*) malloc(sizeof(pkt_t));
    if (pkt == NULL) {
      perror("Erreur lors du malloc du package");
      return NULL;
    }
    pkt->TYPE = 1;
    pkt->TR = 0;
    pkt->WINDOW = 0;
    pkt->LENGTH = htons(0);
    pkt->TIMESTAMP = 0;
    pkt->CRC1 = 0;
    return pkt
}

void pkt_del(pkt_t *pkt)
{
    free(pkt->PAYLOAD);
    free(pkt);
}

pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_encode(const pkt_t* pkt, char *buf, size_t *len)
{
    /* Your code will be inserted here */
}

ptypes_t pkt_get_type  (const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint8_t  pkt_get_tr(const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint8_t  pkt_get_window(const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint8_t  pkt_get_seqnum(const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint16_t pkt_get_length(const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint32_t pkt_get_timestamp   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint32_t pkt_get_crc1   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

uint32_t pkt_get_crc2   (const pkt_t* pkt)
{
    /* Your code will be inserted here */
}

const char* pkt_get_payload(const pkt_t* pkt)
{
    /* Your code will be inserted here */
}


pkt_status_code pkt_set_type(pkt_t *pkt, const ptypes_t type)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_tr(pkt_t *pkt, const uint8_t tr)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_window(pkt_t *pkt, const uint8_t window)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_seqnum(pkt_t *pkt, const uint8_t seqnum)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_length(pkt_t *pkt, const uint16_t length)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_timestamp(pkt_t *pkt, const uint32_t timestamp)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_crc1(pkt_t *pkt, const uint32_t crc1)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_crc2(pkt_t *pkt, const uint32_t crc2)
{
    /* Your code will be inserted here */
}

pkt_status_code pkt_set_payload(pkt_t *pkt,
                                const char *data,
                                const uint16_t length)
{
    /* Your code will be inserted here */
}


ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)
{
    /* Your code will be inserted here */
}


ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)
{
    /* Your code will be inserted here */
}

size_t varuint_len(const uint8_t *data)
{
    /* Your code will be inserted here */
}


ssize_t varuint_predict_len(uint16_t val)
{
    /* Your code will be inserted here */
}


ssize_t predict_header_length(const pkt_t *pkt)
{
    /* Your code will be inserted here */
}
