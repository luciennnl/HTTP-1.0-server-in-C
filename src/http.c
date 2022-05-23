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
    // Check that the last token exists and is HTTP/1.0 as this is the only version we accept"
    /**
     * @brief We will need to perform this check in multiple iterations. First we need to check token exists.
     * Second we need to check that the token begins with HTTP/1.0.
     * Finally, we need to check that there is no additional text beyond apart from \r\n to indicate the end of the first header.
     */
    token = strtok(NULL, WHITESPACE_TOKEN);
    if (token == NULL 
        || strlen(token) < (strlen(HTTP_V) + strlen("\r\n")) 
        || memcmp(token, HTTP_V, strlen(HTTP_V)) != 0 
        || memcmp(token + strlen(HTTP_V), "\r\n", strlen("\r\n")) != 0) {
        return NULL;
    }
    return http_request_constructor(method, resource);
}
http_response *get_response_404() {
    http_response *response = http_response_constructor(HTTP_RESPONSE_404_STATUS_LINE);
    if (!response) {
        fprintf(stderr, "http.c - get_response_404() - Failed to create response");
        exit(1);
    }
    return response;
}
http_response *get_response_200(FILE *f, char *path) {
    http_response *response = http_response_constructor(HTTP_RESPONSE_200_STATUS_LINE);
    if (!response) {
        fprintf(stderr, "http.c - retrieve_file_contents() - malloc failed for http_response...\n");
        return NULL;
    }
    // Get headers
    char *content_type = get_content_type(path);
    // Insert Content-Type header
    insert_response_header(response, HTTP_RESPONSE_CONTENT_TYPE_HEADER, content_type);
    // Get body
    response->body = retrieve_file_contents(f, &response->body_len);
    return response;
}
void insert_response_header(http_response *http_response, char *name, char *value) {
    if (!http_response) {
        return;
    }
    if (!http_response->headers) {
        http_response->headers = http_response_header_constructor(name, value);
    } else {
        http_response_header *curr = http_response->headers;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = http_response_header_constructor(name, value);
    }
}
char *transform_to_absolute_path(char *resource) {
    char *path = malloc(strlen(resource) + strlen(web_root_path) + 1);
    if (!path) {
        fprintf(stderr, "http.c - transform_to_absolute_path() - malloc failed for char *path\n");
        exit(1);
    }
    // Check if path is illegal first
    if (is_illegal_path(resource)) {
        free(path);
        return NULL;
    }
    // Combine web root path with resource path
    memcpy(path, web_root_path, strlen(web_root_path));
    memcpy(path + strlen(web_root_path), resource, strlen(resource));
    path[strlen(web_root_path) + strlen(resource)] = '\0';
    return path;
}
bool is_illegal_path(char* path) {
    char* ret = path;
    while ((ret = strstr(ret, ILLEGAL_PATH_SUBSTRING)) != NULL) {
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

    // If no extension, return application/octet-stream as specified in the specifications
    if (!extension) {
        return CONTENT_TYPE_OCTET_STREAM;
    }
    // Otherwise, try matching the extension with the defined values
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
http_response *http_response_constructor(char *status_line) {
    http_response *response = malloc(sizeof(http_response));
    if (!response) {
        fprintf(stderr, "http.c - http_response_constructor() - malloc failed for http_response\n");
    }
    response->status_line = malloc(strlen(status_line) + 1);
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
        exit(1);
    }
    header->name = malloc(strlen(name) + 1);
    if (!header->name) {
        fprintf(stderr, "http.c - http_response_header_constructor() - malloc failed for http_response_header->name\n");
        exit(1);
    }
    strcpy(header->name, name);
    header->value = malloc(strlen(value) + 1);
    if (!header->value) {
        fprintf(stderr, "http.c - http_response_header_constructor() - malloc failed for http_response_header->value\n");
        exit(1);
    }
    strcpy(header->value, value);
    header->next = NULL;
    return header;
}
void http_response_header_destructor(http_response_header *response_header) {
    if (!response_header) {
        return;
    }
    http_response_header *curr = response_header;
    while (curr != NULL) {
        if (curr->name) {
            free(curr->name);
        }
        if (curr->value) {
            free(curr->value);
        }
        curr = curr->next;
    }
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