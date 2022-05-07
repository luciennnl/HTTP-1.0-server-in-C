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

http_response *http_get(char* req, int reqlen);
void *http_get_string_adaptor(long *response_len, char *req, int reqLen);
http_response *get_response_404();
http_response *get_response_200(FILE *f, char *path);
char *get_response_200_header(char *content_type);
char *parse_requested_path(char *req, int reqLen);
char *get_content_type(char *path);
char *retrieve_file_contents_text(FILE *f, long *file_len);
unsigned char *retrieve_file_contents_binary(FILE *f, long *file_len);
http_response *http_response_constructor();
void http_response_destructor(http_response *response);
#endif
