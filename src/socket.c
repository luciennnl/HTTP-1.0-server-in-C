#include "../header/socket.h"

socket_info *socket_init(protocol_number protocol_no, char *port) {
    int sockfd, re = 1;
    socket_info *out = malloc(sizeof(socket_info));
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    // Assign family based on configuration
    hints.ai_family = protocol_no == IPV6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res;

    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        fprintf(stdout, "server.c - socket_init() - Failed to initialize socket (getaddrinfo failed)...\n");
    };

    // Get addrinfo based on hints
    struct addrinfo *addr = get_socket_of_family_type(res, hints.ai_family);

    if ((sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
        fprintf(stderr, "server.c - socket_init() - Failed to create socket from desired family type\n");
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re)) != 0) {
        fprintf(stderr, "server.c - socket_init() - Failed to set socket options (setsockopt() failed)...\n");
    }
    
    out->addr = addr;
    out->fd = sockfd;
    
    return out;
}
void socket_free(socket_info *socket) {
    free(socket->addr);
    free(socket);
}
struct addrinfo *get_socket_of_family_type(struct addrinfo *addr, int family_type) {
    struct addrinfo *p = addr;
    // Find the address with the desired family type
    while (p != NULL) {
        if (p->ai_family == family_type) {
            break;
        }
        p = p->ai_next;
    }
    if (!p) {
        fprintf(stderr, "server.c - get_socket_of_family_type() - Failed to find socket of desired family type\n");
        exit(1);
    }
    return p;
}
void socket_listen(int sockfd, struct addrinfo *addr) {
    if (bind(sockfd, addr->ai_addr, addr->ai_addrlen) != 0) {
        fprintf(stderr, "server.c - socket_listen() - Failed to bind socket, exiting...\n");
        exit(1);
    };
    if (listen(sockfd, SOCKET_BACKLOG) != 0) {
        fprintf(stderr, "server.c - socket_listen() - Failed to listen socket, exiting...\n");
        exit(1);
    }
}