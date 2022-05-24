#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "./http_domain.h"
#include "./util.h"

#define HTTP_V "HTTP/1.0"
#define HTTP_GET "GET"

#define HTTP_RESPONSE_404_STATUS_LINE "HTTP/1.0 404 Not Found"
#define HTTP_RESPONSE_200_STATUS_LINE "HTTP/1.0 200 OK"
#define HTTP_RESPONSE_CONTENT_TYPE_HEADER "Content-Type"

#define WHITESPACE_TOKEN " "

#define ILLEGAL_PATH_SUBSTRING "/.."
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

/**
 * @brief Function for creating a http response from a http request
 * 
 * @param req The incoming http request
 * @return http_response* The http response to the request
 */
http_response *http_get(char* req);

/**
 * @brief Function for parsing a string into a http_request struct
 * 
 * @param req The string to parse
 * @return http_request* a http_request struct if the string is a valid http request, otherwise NULL
 */
http_request *http_parse_request(char *req);

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
 * @brief Function to insert a http_response_header into the headers property in a http_response struct object
 * 
 * @param http_response The http_response to insert the header
 * @param name The name of the header
 * @param value The value of the header
 */
void insert_response_header(http_response *http_response, char *name, char *value);
/**
 * @brief Function to transform a requested path in a HTTP request to an absolute path on the web server.
 *        Eg. /index.html -> /home/www/index.html
 * 
 * @param resource A string containing the requested resource specified in the HTTP request
 * @return char* The path to the desired resource specified in the request
 */
char *transform_to_absolute_path(char *resource);

/**
 * @brief Function to determine if a provided path is illegal. That is, if it contains the "/.." token.
 * 
 * @param path The path to analyze
 * @return true If the path is illegal
 * @return false If the path is OK
 */
bool is_illegal_path(char* path);

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
 * @brief Function to parse a specified file into an array of chars. 
 *        INFO: You can use this function to parse both text and binary data
 * 
 * @param f a FILE pointer to the target file to be parsed
 * @param file_len a pointer to a long used to store the length of the parsed file
 * @return unsigned char* The output of the parsed file
 */
char *retrieve_file_contents(FILE *f, long *file_len);
#endif
