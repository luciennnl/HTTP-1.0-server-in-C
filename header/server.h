#ifndef SERVER_H

#define SERVER_H

#define _POSIX_C_SOURCE 200112L
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "./socket.h"

#define EXPECTED_INPUT_CNT 3
#define PROTOCOL_NO_ARG_NO 1
#define PORT_ARG_NO 2
#define PATH_TO_WEB_ROOT_ARG_NO 3
char* web_root_path;
struct config {
    protocol_number protocol_no;
    char *port;
    char *path_to_web_root;
};

typedef struct config config;

uint64_t testRequestHandler(char **response, char *argv, int arg);
config *read_input(int argc, char *argv[]);

#endif