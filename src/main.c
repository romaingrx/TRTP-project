#include "packet_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>
#include <errno.h>

char *hostname = NULL, *port = NULL, *filename = NULL, *format = NULL;
size_t connexions;

void err_malloc(char *text){
    fprintf(stderr, "[main] Pas réussi à allouer de la mémoire pour %s.\n", text);
}

void del(){
    if(hostname != NULL){free(hostname);}
    if(port != NULL){free(port);}
    if(filename != NULL){free(filename);}
    if(format != NULL){free(format);}
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Need at least the hostname and the port.\n");
        fprintf(stderr, "[main] Pas reçu assez d'arguments (hostanme et port).\n");
        return EXIT_SUCCESS; // A CHANGER EN EXIT_FAILURE
    }

    int opt;
    while ((opt = getopt(argc, argv, "f:o:m:")) != -1) {
        switch (opt) {
            case 'f':
                filename = malloc(sizeof(optarg));
                if(filename == NULL){err_malloc("FILENAME"); return EXIT_FAILURE;}
                strcpy(filename, optarg);
                printf("FILENAME : %s\n", filename);
                break;
            case 'o':
                format = malloc(sizeof(optarg));
                if(format == NULL){err_malloc("FORMAT"); return EXIT_FAILURE;}
                strcpy(format, optarg);
                printf("FORMAT : %s\n", format);
                break;
            case 'm':
                connexions = atoi(optarg);
                printf("CONNEXIONS : %zu\n", connexions);
                break;
        }
    }
    hostname = malloc(sizeof(argv[optind]));
    if(hostname == NULL){err_malloc("HOSTNAME"); return EXIT_FAILURE;}
    strcpy(hostname, argv[optind]);
    port = malloc(sizeof(argv[optind+1]));
    if(port == NULL){err_malloc("PORT"); return EXIT_FAILURE;}
    strcpy(port, argv[optind+1]);

    printf("HOSTNAME : %s\n", hostname);
    printf("PORT : %s\n", port);
    del();
    return 0;
}
