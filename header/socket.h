#ifndef SOCKET_H
#define SOCKET_H

#define SOCKET_BACKLOG 10

#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
enum protocol_number {
    IPV4 = 4,
    IPV6 = 6
};

typedef enum protocol_number protocol_number;

struct socket_info {
    struct addrinfo *addr;
    int fd;
};

typedef struct socket_info socket_info;

socket_info *socket_init(protocol_number protocol_no, char *port);
void socket_free(socket_info *socket);
struct addrinfo *get_socket_of_family_type(struct addrinfo *addr, int family_type);
void socket_listen(int sockfd, struct addrinfo *addr);
void socket_handle_messages(int sockfd, int client_max, char *(*response_func)(uint64_t*, char*, int));
#endif