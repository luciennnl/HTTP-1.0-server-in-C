#ifndef HTTP_DOMAIN_H
#define HTTP_DOMAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "util.h"

#define HTTP_HEADER_SEPARATOR "\r\n"
#define HTTP_HEADER_KEY_VALUE_SEPARATOR ": "

struct http_response_header {
    char *name;
    char *value;
    struct http_response_header *next;
};

typedef struct http_response_header http_response_header;

struct http_response {
    http_response_header *headers;
    char *status_line;
    char *body;
    long body_len;
};

typedef struct http_response http_response;

struct http_request {
    char *method;
    char *resource;
    // Would need more properties such as body and headers if we were to implement even more functionalities (eg. allowing POST)
};

typedef struct http_request http_request;

/**
 * @brief Function to get the length of a http_response when converted to raw bytes
 * 
 * @param response The http_response to analyze
 * @return long The length of the given http_response
 */
long http_response_length(http_response *response);
/**
 * @brief Function to create a http_response struct, with default values of:
 *      len: 0
 *      content_type: NULL
 *      content: NULL
 * 
 * @return http_response* The created http_response struct 
 */
http_response *http_response_constructor();

/**
 * @brief Function to free a http_response struct including any of the strings contained within.
 * 
 * @param response The target http_response struct
 */
void http_response_destructor(http_response *response);

/**
 * @brief Function to create a http_response_header struct
 * 
 * @param name The name of the header. Eg. (Host, User-Agent, Connection);
 * @param value The value of the header. Eg. (www.example.com, Mozilla/5.0, close)
 * @return http_response_header* The created http_response_header
 */
http_response_header *http_response_header_constructor(char *name, char *value);
/**
 * @brief Function to free a http_response_header struct
 * 
 * @param response_header The http_response_header to free
 */
void http_response_header_destructor(http_response_header *response_header);
/**
 * @brief Function to create a http_request struct, with a specified method indicating the HTTP method (GET/POST/PUT...)
 *        and a resource indicating the path of the requested resource as specified in the second argument of the first line: eg. "/index.html"
 * 
 * @param method The HTTP method
 * @param resource The path of the requested resource
 * @return http_request* The created http_request struct
 */
http_request *http_request_constructor(char *method, char *resource);

/**
 * @brief Function to free a http_request struct including any of the strings contained within
 * 
 * @param request The http_request struct to free
 */
void http_request_destructor(http_request *request);

#endif