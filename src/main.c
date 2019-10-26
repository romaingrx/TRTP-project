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

    if (argc < 3) {
        printf("Need at least the hostname and the port.\n");
        fprintf(stderr, "[main] Pas reçu assez d'arguments (hostanme et port).\n");
        return EXIT_SUCCESS; // A CHANGER EN EXIT_FAILURE
    }

    int opt;
    while ((opt = getopt(argc, argv, "f:o:m:w:")) != -1) {
        switch (opt) {
            case 'f':
                fprintf(stderr, "[main] Le serveur ne prend pas -f comme argument.\n");
                break;
            case 'o':
                format = optarg;
                //printf("FORMAT : %s\n", format);
                break;
            case 'm':
                connections = atoi(optarg);
                //printf("Connections : %d\n", connections);
                break;
            case 'w':
                boolean_window = atoi(optarg);
                break;
            case '?':
                fprintf(stderr, "Argument invalide \n");
        }
    }
    if(format == NULL){
        format = "File %d.txt";
    }

    hostname = argv[optind];
    port = (int)atoi(argv[optind+1]);

    // printf("HOSTNAME : %s\n", hostname);
    // printf("PORT : %d\n", port);
    socket_listening(hostname, port, connections, format, boolean_window);
    return 0;
}
