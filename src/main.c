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

void all_free(char * hostname,char * format){
    if(hostname != NULL){free(hostname);}
    if(format != NULL){free(format);}
}

int main(int argc, char **argv)
{
    char *hostname = NULL, *format = NULL;
    int connections = -1, port;

    if (argc < 3) {
        printf("Need at least the hostname and the port.\n");
        fprintf(stderr, "[main] Pas reçu assez d'arguments (hostanme et port).\n");
        return EXIT_SUCCESS; // A CHANGER EN EXIT_FAILURE
    }

    int opt;
    while ((opt = getopt(argc, argv, "f:o:m:")) != -1) {
        switch (opt) {
            case 'f':
                fprintf(stderr, "[main] Le serveur ne prend pas -f comme argument.\n");
                break;
            case 'o':
                format = malloc(sizeof(optarg));
                if(format == NULL){err_malloc("FORMAT"); return EXIT_FAILURE;}
                strcpy(format, optarg);
                //printf("FORMAT : %s\n", format);
                break;
            case 'm':
                connections = atoi(optarg);
                //printf("Connections : %d\n", connections);
                break;
        }
    }
    if(format == NULL){
        format = malloc(sizeof(char)*11);
        strcpy(format, "File %d.txt");
    }

    hostname = malloc(sizeof(argv[optind]));
    if(hostname == NULL){err_malloc("HOSTNAME"); return EXIT_FAILURE;}
    strcpy(hostname, argv[optind]);
    port = (int)atoi(argv[optind+1]);



    // printf("HOSTNAME : %s\n", hostname);
    // printf("PORT : %d\n", port);
    socket_listening(hostname, port, connections, format);
    all_free(hostname, format);
    return 0;
}
