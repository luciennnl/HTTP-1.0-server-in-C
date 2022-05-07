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
#define HTTP_RESPONSE_404 "HTTP/1.0 404 Not Found\r\n\r\n"
#define HTTP_RESPONSE_200 "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n"
#define HTTP_RESPONSE_200_MAX_LEN 1024
extern char* web_root_path;

char *http_get(uint64_t *response_len, char *req, int reqlen);
char *parse_requested_path(char *req, int reqLen);
char *generate_response_404(uint64_t *response_len);
char *retrieve_file_contents(FILE *f, uint64_t *response_len);
#endif
