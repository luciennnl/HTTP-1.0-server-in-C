#include "../header/server.h"

int main(int argc, char *argv[]) {
    config *cfg = read_input(argc, argv);

    socket_info *socket = socket_init(cfg);
    
    socket_listen(socket->fd, socket->addr);

    close(socket->fd);

    socket_free(socket);
    free(cfg);
    return 0;
}

config *read_input(int argc, char *argv[]) {
    if (argc != EXPECTED_INPUT_CNT + 1) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected 3 arguments but got %d\n", argc);
        exit(1);
    }
    
    config *res = malloc(sizeof(config));
    
    int protocol = atoi(argv[PROTOCOL_ARG_NO]);
    if (protocol != IPV4_NO && protocol != IPV6_NO) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected either 4 (IPV4) or 6 (IPV6) for the first argument\n");
        exit(1);
    }
    res->protocol = protocol;
    res->port = argv[PORT_ARG_NO];
    res->path_to_web_root = argv[PATH_TO_WEB_ROOT_ARG_NO];

    return res;
}
socket_info *socket_init(config *cfg) {
    int sockfd, re = 1;
    socket_info *out = malloc(sizeof(socket_info));
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    // Assign family based on configuration
    hints.ai_family = cfg->protocol == IPV6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res;

    if (getaddrinfo(NULL, cfg->port, &hints, &res) != 0) {
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