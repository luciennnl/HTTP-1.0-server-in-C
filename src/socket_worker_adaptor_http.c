#include "../header/socket_worker_adaptor_http.h"
void *http_get_string_adaptor(long *response_len, char *req) {
    http_response *response = http_get(req);

    if (!response) {
        return NULL;
    }

    char *raw_response = http_response_to_char_array(response, response_len);
    http_response_destructor(response);
    return raw_response;
}
char *http_response_to_char_array(http_response *response, long *response_len) {
    char *raw_response;
    long _response_len = 0;
    long offset = 0;
    
    if (!response) {
        return NULL;
    }
    http_response_header *header = response->headers;

    // Length of response = length of all headers + length of status line + end of header indicator + body length
    _response_len += strlen(response->status_line) + (strlen(HTTP_HEADER_SEPARATOR) * 2) + response->body_len;
    while (header != NULL) {
        _response_len += strlen(header->name) 
            + strlen(HTTP_HEADER_KEY_VALUE_SEPARATOR) 
            + strlen(header->value) 
            + strlen(HTTP_HEADER_SEPARATOR);
        header = header->next;
    }
    
    raw_response = malloc(_response_len);
    if (!raw_response) {
        fprintf(stderr, "http.c - http_response_to_char_array() - malloc failed for raw_response\n");
        exit(1);
    }
    /**
     * @brief Get http status line
     */
    memcpy(raw_response, response->status_line, strlen(response->status_line));
    offset += strlen(response->status_line);
    memcpy(raw_response + offset, HTTP_HEADER_SEPARATOR, strlen(HTTP_HEADER_SEPARATOR));
    offset += strlen(HTTP_HEADER_SEPARATOR);

    /**
     * @brief Get headers
     */
    header = response->headers;
    while (header != NULL) {
        memcpy(raw_response + offset, header->name, strlen(header->name));
        offset += strlen(header->name);
        memcpy(raw_response + offset, HTTP_HEADER_KEY_VALUE_SEPARATOR, strlen(HTTP_HEADER_KEY_VALUE_SEPARATOR));
        offset += strlen(HTTP_HEADER_KEY_VALUE_SEPARATOR);
        memcpy(raw_response + offset, header->value, strlen(header->value));
        offset += strlen(header->value);
        memcpy(raw_response + offset, HTTP_HEADER_SEPARATOR, strlen(HTTP_HEADER_SEPARATOR));
        offset += strlen(HTTP_HEADER_SEPARATOR);
        header = header->next;
    }
    memcpy(raw_response + offset, HTTP_HEADER_SEPARATOR, strlen(HTTP_HEADER_SEPARATOR));
    offset += strlen(HTTP_HEADER_SEPARATOR);
    if (response->body_len > 0) {
        memcpy(raw_response + offset, response->body, response->body_len);
    }
    *response_len = _response_len;
    return raw_response;
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