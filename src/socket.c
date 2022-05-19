#include "../header/socket.h"

socket_info *socket_init(protocol_number protocol_no, char *port) {
    int listenfd, re = 1;
    socket_info *out = malloc(sizeof(socket_info));
    struct addrinfo hints;

    if (!out) {
        fprintf(stderr, "http.c - socket_init()- malloc failed for socket_info\n");
        exit(1);
    }
    memset(&hints, 0, sizeof(hints));

    /**
     * This code extract was written with reference to the lecture slides of COMP30023 Semester 1 2022 @ The University of Melbourne
     * Author: Chris Culnane
     * Year: 2022
     */
    // Assign family based on configuration
    hints.ai_family = protocol_no == IPV6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res;

    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        fprintf(stdout, "server.c - socket_init() - Failed to initialize socket (getaddrinfo failed)...\n");
        exit(1);
    };

    // Get addrinfo based on hints
    struct addrinfo *addr = get_socket_of_family_type(res, hints.ai_family);

    if ((listenfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
        fprintf(stderr, "server.c - socket_init() - Failed to create socket from desired family type\n");
        exit(1);
    }

    // Set socket options so that the same port can be reused with no downtime
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &re, sizeof(re)) != 0) {
        fprintf(stderr, "server.c - socket_init() - Failed to set socket options (setsockopt() failed)...\n");
        exit(1);
    }
    out->addr = addr;
    out->fd = listenfd;
    
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
void socket_listen(int listenfd, struct addrinfo *addr) {
    if (bind(listenfd, addr->ai_addr, addr->ai_addrlen) != 0) {
        fprintf(stderr, "server.c - socket_listen() - Failed to bind socket, exiting...\n");
        exit(1);
    };
    free(addr);
    if (listen(listenfd, SOCKET_BACKLOG) != 0) {
        fprintf(stderr, "server.c - socket_listen() - Failed to listen socket, exiting...\n");
        exit(1);
    }
}
void socket_handle_messages(int listenfd, int client_max, char *(*read_func)(int), void *(*response_func)(long*, char*)) {
    
    struct sockaddr_storage client_addr;
    int connfd;
    socklen_t client_addr_size = sizeof(client_addr);
    pthread_t t;
    
    while (true) {
        connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_size);
        // Create worker_args struct to pass required worker parameters into the thread
        worker_args *args = worker_args_constructor(connfd, read_func, response_func);
        // Create the thread, we don't need to store the id as it will automatically exit
        if ((pthread_create(&t, NULL, &socket_worker, args)) != 0) {
            // Close the connection in the case a thread creation failed
            close(connfd);
        } else {
            pthread_detach(t);
        }
    }
}