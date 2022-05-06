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
#define SOCKET_BACKLOG 10

enum protocol_number {
    IPV4 = 4,
    IPV6 = 6
};

typedef enum protocol_number protocol_number;

struct config {
    protocol_number protocol;
    char *port;
    char *path_to_web_root;
};

typedef struct config config;

struct socket_info {
    struct addrinfo *addr;
    int fd;
};

typedef struct socket_info socket_info;
config *read_input(int argc, char *argv[]);
socket_info *socket_init(config *cfg);
void socket_free(socket_info *socket);
struct addrinfo *get_socket_of_family_type(struct addrinfo *addr, int family_type);
void socket_listen(int sockfd, struct addrinfo *addr);
#endif