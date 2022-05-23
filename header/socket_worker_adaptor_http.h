#ifndef SOCKET_WORKER_ADAPTOR_HTTP_H
#define SOCKET_WORKER_ADAPTOR_HTTP_H

#include "./http.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#define READ_BUFFER_SIZE 8192

/**
 * @brief Adaptor function for the interface provided by socket.c/socket_handle_messages()
 *        Wraps around the http_get() function to adhere to the definition of socket_handle_messages()
 *        
 *        Returns the response as a string from http_response.
 * 
 * @param response_len The length of the response
 * @param req The incoming request to be handled
 * @return void* The output response to be sent
 */
void *http_get_string_adaptor(long *response_len, char *req);

char *http_response_to_char_array(http_response *response, long *response_len);
/**
 * @brief Adaptor function for the interface provided by socket.c/socket_handle_messages()
 *        Implements reading of an incoming socket according to the rules defined in RFC 2616 - Hypertext Transfer Protocol
 *        https://datatracker.ietf.org/doc/html/rfc2616
 * 
 *        More specifically, reads until end of header indicated by \r\n\r\n or EOF. (Body is not supported as we only allow GET requests)
 * 
 * @param connfd The socket file descriptor to read messages from
 * @return char* An output string of the message that is read
 */
char *http_read_adaptor(int connfd);

#endif