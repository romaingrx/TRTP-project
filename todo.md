Objectifs
=================
- Aucune fuite de mémoire
- Selective repeat
- Troncation de donnéee
- IPv6


TODO
=================
#### packet_implem.c ####
- [x] ```c
pkt_t* pkt_new()
```
- [x] ```c
void pkt_del(pkt_t*)
```
- [ ] ```c
pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)
```
- [ ] ```c
pkt_status_code pkt_encode(const pkt_t*, char *buf, size_t *len)
```
- [ ] ```c
const char* pkt_get_payload(const pkt_t*)
```
- [ ] ```c
uint32_t pkt_get_crc2(const pkt_t*)
```
- [ ] ```c
pkt_status_code pkt_set_payload(pkt_t*, const char *data, const uint16_t length)
```
- [ ] ```c
pkt_status_code pkt_set_crc2(pkt_t*, const uint32_t crc2)
```
- [ ] ```c
ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)
```
- [ ] ```c
ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)
```
- [ ] ```c
size_t varuint_len(const uint8_t *data)
```
- [ ] ```c
ssize_t varuint_predict_len(uint16_t val)
```
- [ ] ```c
ssize_t predict_header_length(const pkt_t *pkt)
```
- [ ] **Setters**
  - [x] ```c
  pkt_status_code pkt_set_type (pkt_t*, const ptypes_t type)
  ```
  - [ ] ```c
  cpkt_status_code pkt_set_tr (pkt_t*, const uint8_t tr)
  ```
    - Dans quel cas c'est une erreur entre TR et TYPE
  - [x] ```c
  pkt_status_code pkt_set_window (pkt_t*, const uint8_t window)
  ```
  - [x] ```c
  pkt_status_code pkt_set_seqnum (pkt_t*, const uint8_t seqnum)
  ```
  - [x] ```c
  pkt_status_code pkt_set_length (pkt_t*, const uint16_t length)
  ```
  - [ ] ```c
  pkt_status_code pkt_set_timestamp (pkt_t*, const uint32_t timestamp)
  ```
  - [ ] ```c
  pkt_status_code pkt_set_crc1 (pkt_t*, const uint32_t crc1)
  ```
  - [ ] ```c
  pkt_status_code pkt_set_payload (pkt_t*, const char *data, const uint16_t length)
  ```
  - [ ] ```c
  pkt_status_code pkt_set_crc2 (pkt_t*, const uint32_t crc2)
  ```
- [ ] **Getters**
  - [ ] ```c
  ptypes_t pkt_get_type (const pkt_t*)
  ```
  - [ ] ```c
  uint8_t  pkt_get_tr (const pkt_t*)
  ```
  - [ ] ```c
  uint8_t  pkt_get_window (const pkt_t*)
  ```
  - [ ] ```c
  uint8_t  pkt_get_seqnum (const pkt_t*)
  ```
  - [ ] ```c
  uint16_t pkt_get_length (const pkt_t*)
  ```
  - [ ] ```c
  uint32_t pkt_get_timestamp (const pkt_t*)
  ```
  - [ ] ```c
  uint32_t pkt_get_crc1 (const pkt_t*)
  ```
  - [ ] ```c
  const char* pkt_get_payload (const pkt_t*)
  ```
  - [ ] ```c
  uint32_t pkt_get_crc2 (const pkt_t*)
  ```

Improvements
=================
