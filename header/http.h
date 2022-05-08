#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXIMUM_PATH_STR_LEN 2048
#define MAXIMUM_REQUEST_PATH_STR_LEN 1024
#define INITIAL_RESPONSE_BODY_BUFFER_SIZE 1024
#define HTTP_RESPONSE_404_HEADER "HTTP/1.0 404 Not Found\r\n\r\n"
#define HTTP_RESPONSE_200_HEADER "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n"
#define HTTP_RESPONSE_200_HEADER_MAX_LEN 1024

#define FILE_EXTENSION_HTML ".html"
#define FILE_EXTENSION_CSS ".css"
#define FILE_EXTENSION_JPEG ".jpg"
#define FILE_EXTENSION_JS ".js"

#define CONTENT_TYPE_HTML "text/html"
#define CONTENT_TYPE_CSS "text/css"
#define CONTENT_TYPE_JPEG "image/jpeg"
#define CONTENT_TYPE_JS "application/javascript"
#define CONTENT_TYPE_OCTET_STREAM "application/octet-stream"

extern char* web_root_path;

struct http_response {
    uint64_t len;
    char *content_type;
    char *content;
};

typedef struct http_response http_response;

/**
 * @brief Function for creating a http response from a http request
 * 
 * @param req The incoming http request
 * @param reqlen The length of the incoming request
 * @return http_response* The http response to the request
 */
http_response *http_get(char* req, int reqlen);
/**
 * @brief Adaptor function for the interface provided by socket.c/socket_handle_messages()
 *        Wraps around the http_get() function to adhere to the definition of socket_handle_messages()
 *        
 *        Returns the response as a string from http_response.
 * 
 * @param response_len The length of the response
 * @param req The incoming request to be handled
 * @param reqLen The length of the incoming request
 * @return void* The output response to be sent
 */
void *http_get_string_adaptor(long *response_len, char *req, long reqLen);
/**
 * @brief Function to generate a http response of status 404.
 *        Body will not be populated.
 * 
 * @return http_response* The generated http_response struct
 */
http_response *get_response_404();
/**
 * @brief Function to generate a http response of status 200.
 *        Headers will be populated based on get_response_200_headers
 *        Body will be populated with the content of the specified file
 * 
 * @param f The file to be parsed as the body
 * @param path The path to the specified file
 * @return http_response* The generated http_response struct
 */
http_response *get_response_200(FILE *f, char *path);
/**
 * @brief Function to generate a http status 200 response header.
 *        INFO: Currently only supports the HTTP Status & Content-Type headers.
 * 
 * @param content_type The Content-Type header value
 * @return char* The generated http status 200 response headers
 */
char *get_response_200_headers(char *content_type);
/**
 * @brief Function to extract the path to the requested resource from a http request.
 *        EG. GET /index.html HTTP/1.0 -> "/index.html"
 * 
 * @param req A string representation of the request
 * @param reqLen The length of the request string
 * @return char* The path to the desired resource specified in the request
 */
char *parse_requested_path(char *req, int reqLen);
/**
 * @brief Function to parse a path to a file and determine the content type.
 *        Determines content-type based on the file extension
 * 
 * @param path The path to be parsed and analysed
 * @return char* One of:
 *         - CONTENT_TYPE_HTML "text/html"
 *         - CONTENT_TYPE_CSS "text/css"
 *         - CONTENT_TYPE_JPEG "image/jpeg"
 *         - CONTENT_TYPE_JS "application/javascript"
 *         - CONTENT_TYPE_OCTET_STREAM "application/octet-stream"
 *              
 */
char *get_content_type(char *path);
/**
 * @brief Function to parse a specified file into an array of unsigned chars. 
 *        Use this function when parsing normal text files.
 * 
 * @param f a FILE pointer to the target file to be parsed
 * @param file_len a pointer to a long used to store the length of the parsed file
 * @return char* The output of the parsed file
 */
char *retrieve_file_contents_text(FILE *f, long *file_len);
/**
 * @brief Function to parse a specified file into an array of unsigned chars. 
 *        Use this function when parsing binary files as unsigned char better supports the storage of binary data.
 * 
 * @param f a FILE pointer to the target file to be parsed
 * @param file_len a pointer to a long used to store the length of the parsed file
 * @return unsigned char* The output of the parsed file
 */
unsigned char *retrieve_file_contents_binary(FILE *f, long *file_len);
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
#endif
