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
/**
 * @brief Function to free the memory allocated for a socket_info struct
 * 
 * @param socket The target socket_info struct
 */
void socket_free(socket_info *socket);

/**
 * @brief Get the socket of the specified family type from a linked list of struct addrinfo
 * 
 * @param addr The linked list of struct addrinfo containing the potential candidate
 * @param family_type The desired family type. Eg. AF_INET, AF_INET6
 * @return struct addrinfo* The socket address struct that matches the desired family_type
 */
struct addrinfo *get_socket_of_family_type(struct addrinfo *addr, int family_type);
/**
 * @brief Wrapper function for setting up a socket (binding and listening), does not accept incoming requests.
 * 
 * @param sockfd The file descriptor for the listening half socket to be setup
 * @param addr An addrinfo struct describing the details of the socket address where this socket will be setup
 */
void socket_listen(int sockfd, struct addrinfo *addr);

/**
 * @brief Template function for handling incoming messages into the specified socket, requires a (response_func)
 *        which will perform the request processing and the response will be based on the output of this function.
 * 
 * @param sockfd The file descriptor for the listening half socket where incoming connections and requested will be handled
 * @param client_max The max number of clients that can be handled concurrently by the socket
 * @param response_func The handler function for incoming messages - input parameters (long *resLen, char *req, long reqLen):
 *                      resLen: A pointer to a long describing the length of the response returned by the function. 0 if NULL
 *                      req: A string containing the request message
 *                      reqLen: A long describing the length of the request message
 */
void socket_handle_messages(int sockfd, int client_max, void *(*response_func)(long*, char*, long));
#endif