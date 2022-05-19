#include "../header/http.h"
http_response *http_get(char* req) {
    http_request *request;
    http_response *response;
    FILE *f;
    char *path;
    
    if ((request = http_parse_request(req)) == NULL) {
        return NULL;
    }
    path = transform_to_absolute_path(request->resource);
    f = fopen(path, "r");
    if (path == NULL || f == NULL) {
        response = get_response_404();
    } else {
        response = get_response_200(f, request->resource);
        free(path);
    }
    return response;
}
http_request *http_parse_request(char *req) {
    char *token, *method, *resource;
    // Check that the first token is GET (Since we don't support any other methods yet)
    method = strtok(req, WHITESPACE_TOKEN);
    if (method == NULL || strcmp(method, HTTP_GET) != 0) {
        return NULL;
    }
    // Check that the second token exists and begins with a '/' (The root directory)
    resource = strtok(NULL, WHITESPACE_TOKEN);
    if (resource == NULL || resource[0] != '/') {
        return NULL;
    }
    // Check that the last token exists and is some form of "HTTP/*"
    token = strtok(NULL, WHITESPACE_TOKEN);
    if (token == NULL || memcmp(token, HTTP_V, sizeof(HTTP_V) - 1) != 0) {
        return NULL;
    }
    return http_request_constructor(method, resource);
}
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
    long nbytes, bytes_read = 0, current_size = 8192;
    char *message = malloc(READ_BUFFER_SIZE);
    char buffer[READ_BUFFER_SIZE];
    char *end_of_header;

    if (!message) {
        fprintf(stderr, "http.c - http_read_adaptor() - malloc failed for char *message\n");
        exit(1);
    }
    while ((nbytes = recv(connfd, buffer, sizeof(buffer), 0)) != -1) {
        if (nbytes == 0) {
            break;
        }
        if (bytes_read + nbytes > current_size) {
            message = realloc(message, current_size + READ_BUFFER_SIZE);

            if (!message) {
                fprintf(stderr, "http.c - http_read_adaptor() - realloc failed for *message\n");
                exit(1);
            }
        }
        // We use memcpy as strcpy depends on the '\0' character which may not be read
        memcpy(message + bytes_read, buffer, nbytes);
        bytes_read += nbytes;
        // Check for the end of header indicator as specified in RFC 2616
        if ((end_of_header = strstr(message, "\r\n\r\n")) != NULL || (end_of_header = strstr(message, "\n\n")) != NULL) {
            break;
        }
    }
    return message;
}
http_response *get_response_404() {
    http_response *response = http_response_constructor();
    response->content = malloc(sizeof(HTTP_RESPONSE_404_HEADER));
    if (!response->content) {
        fprintf(stderr, "http.c - get_response_404() - malloc failed for http_response->content\n");
        exit(1);
    }
    response->len = sizeof(HTTP_RESPONSE_404_HEADER);
    memcpy(response->content, HTTP_RESPONSE_404_HEADER, response->len);
    return response;
}
http_response *get_response_200(FILE *f, char *path) {
    http_response *response = http_response_constructor();
    if (!response) {
        fprintf(stderr, "http.c - retrieve_file_contents() - malloc failed for http_response...\n");
        return NULL;
    }
    // Get headers
    char *content_type = get_content_type(path);
    char *header = get_response_200_headers(content_type);

    // Get body
    long body_len;
    void *body;

    /**
     * @brief Need to check if content type is JPEG, if so, we need to treat the data as binary rather than text
     */
    body = retrieve_file_contents(f, &body_len);
    response->content = malloc(strlen(header) + body_len);
    if (response->content) {
        //Combine header and body
        memcpy(response->content, header, strlen(header));
        memcpy(response->content+strlen(header), body, body_len);
        response->len = strlen(header) + body_len;
    } else {
        fprintf(stderr, "http.c - retrieve_file_contents() - malloc failed for http_response->content\n");
        exit(1);
    }
    free(header);
    free(body);
    return response;
}
char *get_response_200_headers(char *content_type) {
    char *header = malloc(HTTP_RESPONSE_200_HEADER_MAX_LEN * sizeof(char));
    if (!header) {
        fprintf(stderr, "http.c - get_response_200_headers() - malloc failed for char *header\n");
        exit(1);
    }
    snprintf(header, HTTP_RESPONSE_200_HEADER_MAX_LEN, HTTP_RESPONSE_200_HEADER, content_type);
    return header;
}
char *transform_to_absolute_path(char *resource) {
    char *path = malloc(strlen(resource) + strlen(web_root_path) + 1);
    if (!path) {
        fprintf(stderr, "http.c - transform_to_absolute_path() - malloc failed for char *path\n");
        exit(1);
    }
    if (is_illegal_path(resource)) {
        free(path);
        return NULL;
    }
    memcpy(path, web_root_path, strlen(web_root_path));
    memcpy(path + strlen(web_root_path), resource, strlen(resource));
    path[strlen(web_root_path) + strlen(resource)] = '\0';
    return path;
}
bool is_illegal_path(char* path) {
    char* ret = path;
    while ((ret = strstr(ret, ILLEGAL_PATH_SUBSTRING)) != NULL) {
        printf("%s\n", ret);
        char next = ret[strlen(ILLEGAL_PATH_SUBSTRING)];
        if (next == '\0' || next == '/') {
            return true;
        }
        ret++;
    }
    return false;
}
char *get_content_type(char *path) {
    const char *extension = strrchr(path, '.');

    if (!extension) {
        return CONTENT_TYPE_OCTET_STREAM;
    }
    if (strcmp(extension, FILE_EXTENSION_HTML) == 0) {
        return CONTENT_TYPE_HTML;
    } else if (strcmp(extension, FILE_EXTENSION_CSS) == 0) {
        return CONTENT_TYPE_CSS;
    } else if (strcmp(extension, FILE_EXTENSION_JPEG) == 0) {
        return CONTENT_TYPE_JPEG;
    } else if (strcmp(extension, FILE_EXTENSION_JS) == 0) {
        return CONTENT_TYPE_JS;
    } else {
        return CONTENT_TYPE_OCTET_STREAM;
    }
}
char *retrieve_file_contents(FILE *f, long *file_len) {
    char *content;

    // Analyze the length of the file
    fseek(f, 0, SEEK_END);
    *file_len = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    content = malloc(*file_len * sizeof(char));
    if (content) {
        fread(content, sizeof(char), *file_len, f);
    } else {
        fprintf(stderr, "http.c - retrieve_file_contents_binary() - malloc failed for char *content...\n");
        exit(1);
    }
    return content;
}
http_response *http_response_constructor() {
    http_response *response = malloc(sizeof(http_response));
    if (!response) {
        fprintf(stderr, "http.c - http_response_constructor() - malloc failed for http_response\n");
    }
    response->content = NULL;
    response->content_type = NULL;
    response->len = 0;

    return response;
}
void http_response_destructor(http_response *response) {
    if (response->content) {
        free(response->content);
    }
    if (response->content_type) {
        free(response->content_type);
    }
    free(response);
}
http_request *http_request_constructor(char *method, char *resource) {
    http_request *request = malloc(sizeof(http_request));
    if (!request) {
        fprintf(stderr, "http.c - http_request_constructor() - malloc failed for http_request\n");
        exit(1);
    }
    request->method = malloc(strlen(method) + 1);
    if (!request->method) {
        fprintf(stderr, "http.c - http_request_constructor()- malloc failed for http_request->method\n");
        exit(1);
    }
    request->resource = malloc(strlen(resource) + 1);
    if (!request->resource) {
        fprintf(stderr, "http.c - http_request_constructor()- malloc failed for http_request->resource\n");
        exit(1);
    }
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