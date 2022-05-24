#ifndef SOCKET_WORKER_ADAPTOR_HTTP_H
#define SOCKET_WORKER_ADAPTOR_HTTP_H

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>

#include "./http.h"
#include "./util.h"

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
void *socket_worker_response_func_adaptor_http(long *response_len, char *req);

/**
 * @brief Function to convert a http_response struct into a char* array
 * 
 *        Converts http_response:
 *              *status-line -> <status_line>CRLF
 *              *headers -> *(<message-header>CRLF)
 *              *body -> <message-body>
 * 
 *        In the form
 *              <status-line>CRLF
 *              *(<message-header>CRLF)
 *              CRLF
 *              <message-body>
 *        As specified in RFC 2616 Fielding, et al.
 * 
 * @param response The http_response struct to be converted
 * @param response_len A pointer which will be updated with the response length
 * @return char* The converted http_response message
 */
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
char *socket_worker_read_func_adaptor_http(int connfd);

#endif