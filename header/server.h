#ifndef SERVER_H

#define SERVER_H

#define _POSIX_C_SOURCE 200112L
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#define EXPECTED_INPUT_CNT 3
#define PROTOCOL_ARG_NO 1
#define PORT_ARG_NO 2
#define PATH_TO_WEB_ROOT_ARG_NO 3
#define IPV4_NO 4
#define IPV6_NO 6

enum protocol_number {
    IPV4 = 4,
    IPV6 = 6
};

typedef enum protocol_number protocol_number;

struct config {
    protocol_number protocol;
    uint16_t port;
    char *path_to_web_root;
};

typedef struct config config;

config *read_input(int argc, char *argv[]);
#endif