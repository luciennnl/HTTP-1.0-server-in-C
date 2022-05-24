#include "../header/http_domain.h"
long http_response_length(http_response *response) {
    long response_len = 0;
    http_response_header *header = response->headers;

    // Length of response = length of all headers + length of status line + end of header indicator + body length
    response_len += strlen(response->status_line) + (strlen(HTTP_HEADER_SEPARATOR) * 2) + response->body_len;
    while (header != NULL) {
        response_len += strlen(header->name) + strlen(HTTP_HEADER_KEY_VALUE_SEPARATOR) + strlen(header->value) 
            + strlen(HTTP_HEADER_SEPARATOR);
        header = header->next;
    }

    return response_len;
}
http_response *http_response_constructor(char *status_line) {
    http_response *response = malloc(sizeof(http_response));
    if (!response) {
        fprintf(stderr, "http.c - http_response_constructor() - malloc failed for http_response\n");
    }
    response->status_line = malloc(strlen(status_line) + 1);
    /**
     * @brief Make a copy instead of assigning the actual string because we could be assigning static data
     *        which would cause a SEG fault when trying to free
     */
    strcpy(response->status_line, status_line);
    response->headers = NULL;
    response->body = NULL;
    response->body_len = 0;

    return response;
}
void http_response_destructor(http_response *response) {
    if (response->status_line) {
        free(response->status_line);
    }
    if (response->headers) {
        http_response_header_destructor(response->headers);
    }
    if (response->body) {
        free(response->body);
    }
    free(response);
}
http_response_header *http_response_header_constructor(char *name, char *value) {
    http_response_header *header = malloc(sizeof(http_response_header));
    if (!header) {
        fprintf(stderr, "http.c - http_response_header_constructor() - malloc failed for http_response_header\n");
        exit(ERROR_STATUS_CODE);
    }
    header->name = malloc(strlen(name) + 1);
    if (!header->name) {
        fprintf(stderr, "http.c - http_response_header_constructor() - malloc failed for http_response_header->name\n");
        exit(ERROR_STATUS_CODE);
    }
    /**
     * @brief Make a copy instead of assigning the actual string because we could be assigning static data
     *        which would cause a SEG fault when trying to free
     */
    strcpy(header->name, name);
    header->value = malloc(strlen(value) + 1);
    if (!header->value) {
        fprintf(stderr, "http.c - http_response_header_constructor() - malloc failed for http_response_header->value\n");
        exit(ERROR_STATUS_CODE);
    }
    // Same reason as above
    strcpy(header->value, value);
    header->next = NULL;
    return header;
}
void http_response_header_destructor(http_response_header *response_header) {
    if (!response_header) {
        return;
    }
    http_response_header *curr = response_header, *temp;
    // Free the linked list
    while (curr != NULL) {
        if (curr->name) {
            free(curr->name);
        }
        if (curr->value) {
            free(curr->value);
        }
        temp = curr;
        curr = curr->next;
        free(temp);
    }
}
http_request *http_request_constructor(char *method, char *resource) {
    http_request *request = malloc(sizeof(http_request));
    if (!request) {
        fprintf(stderr, "http.c - http_request_constructor() - malloc failed for http_request\n");
        exit(ERROR_STATUS_CODE);
    }
    request->method = malloc(strlen(method) + 1);
    if (!request->method) {
        fprintf(stderr, "http.c - http_request_constructor()- malloc failed for http_request->method\n");
        exit(ERROR_STATUS_CODE);
    }
    request->resource = malloc(strlen(resource) + 1);
    if (!request->resource) {
        fprintf(stderr, "http.c - http_request_constructor()- malloc failed for http_request->resource\n");
        exit(ERROR_STATUS_CODE);
    }
    /**
     * @brief Make a copy instead of assigning the actual string because we could be assigning static data
     *        which would cause a SEG fault when trying to free
     */
    strcpy(request->method, method);
    strcpy(request->resource, resource);

    return request;
}
void http_request_destructor(http_request *request) {
    if (request->method) {
        free(request->method);
    }
    if (request->resource) {
        free(request->resource);
    }
    free(request);
}