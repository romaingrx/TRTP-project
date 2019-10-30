#include "packet.h"
#include "receive.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>


void err_malloc(char *text){
    fprintf(stderr, "[main] Pas réussi à allouer de la mémoire pour %s.\n", text);
}


int main(int argc, char **argv)
{
    char *hostname = NULL, *format = NULL;
    int connections = -1, port, boolean_window = 0;
    timeout = 20;//Défini au cas ou. :P
    log_out = 0;
    int opt;
    while ((opt = getopt(argc, argv, "f:o:m:t:lw")) != -1) {
        switch (opt) {
            case 'f':
                fprintf(stderr, "[main] Le serveur ne prend pas -f comme argument.\n");
                break;
            case 'o':
                format = optarg;
                break;
            case 'm':
                connections = atoi(optarg);
                break;
            case 't':
                timeout = atoi(optarg);
                break;
            case 'w':
                boolean_window = 1;
                break;
            case 'l':
                log_out = 1;
                break;
            case '?':
                fprintf(stderr, "Argument invalide : %s\n", argv[optind-1]);
        }
    }
    if(format == NULL){
        format = "File %d.txt";
    }
    if(argc-optind < 2){
        fprintf(stderr, "[main] Pas reçu assez d'arguments (hostanme et port).\n");
        exit(EXIT_FAILURE);
    }

    hostname = argv[optind];
    port = (int)atoi(argv[optind+1]);

    socket_listening(hostname, port, connections, format, boolean_window);
    return 0;
}
