Objectifs
=================
- Aucune fuite de mémoire
- Selective repeat
- Troncation de donnéee
- IPv6


TODO
=================
#### packet_implem.c ####
- [x] ```pkt_t* pkt_new()```
- [ ] ```void pkt_del(pkt_t*)```
- [ ] ```pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)```
- [ ] ```pkt_status_code pkt_encode(const pkt_t*, char *buf, size_t *len)```
- [ ] ```const char* pkt_get_payload(const pkt_t*)```
- [ ] ```uint32_t pkt_get_crc2(const pkt_t*)```
- [ ] ```pkt_status_code pkt_set_payload(pkt_t*,
                                const char *data,
                                const uint16_t length)```
- [ ] ```pkt_status_code pkt_set_crc2(pkt_t*, const uint32_t crc2)```
- [ ] ```ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)```
- [ ] ```ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)```
- [ ] ```size_t varuint_len(const uint8_t *data)```
- [ ] ```ssize_t varuint_predict_len(uint16_t val)```
- [ ] ```ssize_t predict_header_length(const pkt_t *pkt)```

Improvements
=================
