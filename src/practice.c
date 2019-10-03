#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <zlib.h>
#include <arpa/inet.h>

int main(int argc, char const *argv[]) {
  pkt_t *pkt = pkt_new();
  if(pkt_set_length(pkt, -1) != PKT_OK){printf("%s\n","NOT DONE");}
  printf("%u\n",pkt_get_length(pkt));
  pkt_del(pkt);
  return 0;
}
