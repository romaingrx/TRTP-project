Objectifs
=================
- Aucune fuite de memoire
- Selective repeat
- Troncation de donnee
- IPv6


TODO
=================

#### Global ####

<!-- ```c
listen(int sockfd, int backlog);
```
- backlog donne le nombre d'entrées maximales à recevoir. -->

#### Progs ####


<details><summary>packet_implem.c</summary>

- [x] ```pkt_t* pkt_new()```
- [x] ```void pkt_del(pkt_t*)```
- [ ] ```pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)```
  - Comment prendre en compte LENGTH coder sur 7 ou 15 bits en fonction de L
  - Rajouter les conditions pour lesquels on prend en compte ou pas (TR, LENGTH, ...)
- [ ] ```pkt_status_code pkt_encode(const pkt_t*, char *buf, size_t *len)```
- [ ] ```const char* pkt_get_payload(const pkt_t*)```
- [ ] ```uint32_t pkt_get_crc2(const pkt_t*)```
- [ ] ```pkt_status_code pkt_set_payload(pkt_t*, const char *data, const uint16_t length)```
- [ ] ```pkt_status_code pkt_set_crc2(pkt_t*, const uint32_t crc2)```
- [x] ```ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)```
- [x] ```ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)```
- [x] ```size_t varuint_len(const uint8_t *data)```
- [x] ```ssize_t varuint_predict_len(uint16_t val)```
- [x] ```ssize_t predict_header_length(const pkt_t *pkt)```
  <details><summary>Setters</summary>

  - [x] ```pkt_status_code pkt_set_type     (pkt_t*, const ptypes_t type)```
  - [x] ```pkt_status_code pkt_set_tr       (pkt_t*, const uint8_t tr)```
  - [x] ```pkt_status_code pkt_set_window   (pkt_t*, const uint8_t window)```
  - [x] ```pkt_status_code pkt_set_seqnum   (pkt_t*, const uint8_t seqnum)```
  - [x] ```pkt_status_code pkt_set_length   (pkt_t*, const uint16_t length)```
  - [x] ```pkt_status_code pkt_set_timestamp(pkt_t*, const uint32_t timestamp)```
  - [x] ```pkt_status_code pkt_set_crc1     (pkt_t*, const uint32_t crc1)```
  - [x] ```pkt_status_code pkt_set_payload(pkt_t*, const char *data, const uint16_t length)```
  - [x] ```pkt_status_code pkt_set_crc2(pkt_t*, const uint32_t crc2)```
  </details>
  <details><summary>Getters</summary>

  - [x] ```ptypes_t pkt_get_type (const pkt_t*)```
  - [x] ```uint8_t  pkt_get_tr (const pkt_t*)```
  - [x] ```uint8_t  pkt_get_window (const pkt_t*)```
  - [x] ```uint8_t  pkt_get_seqnum (const pkt_t*)```
  - [x] ```uint16_t pkt_get_length (const pkt_t*)```
  - [x] ```uint32_t pkt_get_timestamp (const pkt_t*)```
  - [x] ```uint32_t pkt_get_crc1 (const pkt_t*)```
  - [x] ```const char* pkt_get_payload (const pkt_t*)```
  - [x] ```uint32_t pkt_get_crc2(const pkt_t*)```
  </details>
  <details><summary>Binary decoders</summary>

  - [x] uint8_t binary_decode_type(uint8_t first_byte)
  - [x] uint8_t binary_decode_tr(uint8_t first_byte)
  - [x] uint8_t binary_decode_window(uint8_t first_byte)
  - [x] uint8_t binary_decode_l(uint16_t length_bytes)
  - [x] uint16_t binary_decode_length(uint16_t length_bytes)
  </details>
  <details><summary>packet_treatment.c</summary>

    - [ ] ```int treat_treat(const pkt_t *)```
    - [ ] ```boolean treat_header_crc1(const pkt_t *)```
    - [ ] ```boolean treat_payload_crc2(const pkt_t *)```
    - [ ] ```int treat_enqueue(const pkt_t *)```
    - [ ] ```pkt_t treat_dequeue(int seqnum)```
    - [ ] ```T'es qu'un porc Domi, tu le mets entre Setters et Getters```

  </details>
</details>

#### Test Code ####


<details><summary>packet_implem.c</summary>

- [ ] ```pkt_t* pkt_new()```
- [ ] ```void pkt_del(pkt_t*)```
- [ ] ```pkt_status_code pkt_decode(const char *data, const size_t len, pkt_t *pkt)```
  - Fonctionne pour le HEADER avec L=1
- [ ] ```pkt_status_code pkt_encode(const pkt_t*, char *buf, size_t *len)```
- [ ] ```const char* pkt_get_payload(const pkt_t*)```
- [ ] ```uint32_t pkt_get_crc2(const pkt_t*)```
- [ ] ```pkt_status_code pkt_set_payload(pkt_t*, const char *data, const uint16_t length)```
- [ ] ```pkt_status_code pkt_set_crc2(pkt_t*, const uint32_t crc2)```
- [x] ```ssize_t varuint_decode(const uint8_t *data, const size_t len, uint16_t *retval)```
- [x] ```ssize_t varuint_encode(uint16_t val, uint8_t *data, const size_t len)```
- [x] ```size_t varuint_len(const uint8_t *data)```
- [x] ```ssize_t varuint_predict_len(uint16_t val)```
- [xﬁ] ```ssize_t predict_header_length(const pkt_t *pkt)```
  <details><summary>Setters</summary>

  - [x] ```pkt_status_code pkt_set_type     (pkt_t*, const ptypes_t type)```
  - [ ] ```pkt_status_code pkt_set_tr       (pkt_t*, const uint8_t tr)```
  - [x] ```pkt_status_code pkt_set_window   (pkt_t*, const uint8_t window)```
  - [x] ```pkt_status_code pkt_set_seqnum   (pkt_t*, const uint8_t seqnum)```
  - [x] ```pkt_status_code pkt_set_length   (pkt_t*, const uint16_t length)```
  - [ ] ```pkt_status_code pkt_set_timestamp(pkt_t*, const uint32_t timestamp)```
  - [ ] ```pkt_status_code pkt_set_crc1     (pkt_t*, const uint32_t crc1)```
  - [ ] ```pkt_status_code pkt_set_payload(pkt_t*, const char *data, const uint16_t length)```
  - [ ] ```pkt_status_code pkt_set_crc2(pkt_t*, const uint32_t crc2)```
  </details>
  <details><summary>Getters</summary>

  - [x] ```ptypes_t pkt_get_type (const pkt_t*)```
  - [ ] ```uint8_t  pkt_get_tr (const pkt_t*)```
  - [x] ```uint8_t  pkt_get_window (const pkt_t*)```
  - [x] ```uint8_t  pkt_get_seqnum (const pkt_t*)```
  - [x] ```uint16_t pkt_get_length (const pkt_t*)```
  - [ ] ```uint32_t pkt_get_timestamp (const pkt_t*)```
  - [ ] ```uint32_t pkt_get_crc1 (const pkt_t*)```
  - [ ] ```const char* pkt_get_payload (const pkt_t*)```
  - [ ] ```uint32_t pkt_get_crc2(const pkt_t*)```
  </details>
  <details><summary>Binary decoders</summary>

  - [ ] ```uint8_t binary_decode_type(uint8_t first_byte)```
  - [ ] ```uint8_t binary_decode_tr(uint8_t first_byte)```
  - [ ] ```uint8_t binary_decode_window(uint8_t first_byte)```
  - [ ] ```uint8_t binary_decode_l(uint16_t length_bytes)```
  - [ ] ```uint16_t binary_decode_length(uint16_t length_bytes)```
  </details>
</details>

#### Incompréhension ####
- `pkt_set_tr` : les conditions pour être accepté
- `pkt_get_crc2` : les conditions pour exister

#### Erreurs ####
- `varuint_encode` et `varuint_decode` pour un LENGTH rempli de 1 (0 vient se mettre au milieu)


#### Improvements ####
- Dans `pkt_decode`, possibilité de `memcpy` le premier byte mais on doit s'assurer que tout est dans les bornes.
- hton sur les uint16_t et uint32_t
- `varuint_encode` : Les deux cas si htons(1) == 1 ou pas
- Lors du décodage, quelle longueur prendre? (Celle dans pkt->LENGTH, un caclul par rapport aux bits reçus, ...)
