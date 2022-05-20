#include "../header/socket_worker_adaptor_http.h"
void *http_get_string_adaptor(long *response_len, char *req) {
    http_response *response = http_get(req);

    if (!response) {
        return NULL;
    }
    // If response content does not exist, free the response object
    if (!response->content) {
        http_response_destructor(response);
        return NULL;
    }
    *response_len = response->len;
    void *full_response = malloc(response->len);
    if (!full_response) {
        fprintf(stderr, "http.c - http_get_string_adaptor() - malloc failed for void *full_response\n");
        exit(1);
    }
    // Save the content into its separate string
    memcpy(full_response, response->content, response->len);
    http_response_destructor(response);
    return full_response;
}
char *http_read_adaptor(int connfd) {
    long nbytes, bytes_read = 0, current_size = READ_BUFFER_SIZE;
    char *message = malloc(READ_BUFFER_SIZE);
    char buffer[READ_BUFFER_SIZE];
    char *end_of_header;

    if (!message) {
        fprintf(stderr, "http.c - http_read_adaptor() - malloc failed for char *message\n");
        exit(1);
    }
    while ((nbytes = recv(connfd, buffer, sizeof(buffer), 0)) != -1) {
        // If EOF, then stop reading
        if (nbytes == 0) {
            break;
        }
        if (bytes_read + nbytes > current_size) {
            message = realloc(message, current_size + READ_BUFFER_SIZE);

            if (!message) {
                fprintf(stderr, "http.c - http_read_adaptor() - realloc failed for *message\n");
                exit(1);
            }
            current_size += READ_BUFFER_SIZE;
        }
        // We use memcpy as strcpy depends on the '\0' character which may not be read
        memcpy(message + bytes_read, buffer, nbytes);
        bytes_read += nbytes;
        // Check for the end of header indicator as specified in RFC 2616
        if ((end_of_header = strstr(message, "\r\n\r\n")) != NULL || (end_of_header = strstr(message, "\n\n")) != NULL) {
            break;
        }
    }
    // Realloc memory in the case of the edge case where there is no additional space for the null terminator
    message = realloc(message, current_size + 1);
    message[bytes_read] = '\0';
    return message;
}