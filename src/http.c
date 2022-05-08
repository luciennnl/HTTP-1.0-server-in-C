#include "../header/http.h"
http_response *http_get(char* req, int reqlen) {
    http_response *response;
    FILE *f;
    char *path = parse_requested_path(req, reqlen);
    f = fopen(path, "r");
    if (f == NULL) {
        response = get_response_404();
    } else {
        response = get_response_200(f, path);
    }
    free(path);
    return response;
}
void *http_get_string_adaptor(long *response_len, char *req, long reqLen) {
    http_response *response = http_get(req, reqLen);
    *response_len = response->len;

    // If response content does not exist, free the response object
    if (!response->content) {
        http_response_destructor(response);
        return NULL;
    }
    void *ret = malloc(response->len);
    // Save the content into its separate string
    memcpy(ret, response->content, response->len);
    http_response_destructor(response);
    return ret;
}
http_response *get_response_404() {
    http_response *response = http_response_constructor();
    response->content = malloc(sizeof(HTTP_RESPONSE_404_HEADER));
    response->len = sizeof(HTTP_RESPONSE_404_HEADER);
    strcpy(response->content, HTTP_RESPONSE_404_HEADER);
    return response;
}
http_response *get_response_200(FILE *f, char *path) {
    http_response *response = http_response_constructor();
    if (!response) {
        fprintf(stderr, "http.c - retrieve_file_contents() - malloc failed for *response...\n");
        return NULL;
    }
    // Get headers
    char *content_type = get_content_type(path);
    char *header = get_response_200_headers(content_type);

    // Get body
    long body_len;
    void *body;
    if (strcmp(content_type, CONTENT_TYPE_JPEG) == 0) {
        body = retrieve_file_contents_binary(f, &body_len);
    } else {
        body = retrieve_file_contents_text(f, &body_len);
    }
    response->content = malloc(strlen(header) + body_len);
    if (response->content) {
        //Combine header and body
        memcpy(response->content, header, strlen(header));
        memcpy(response->content+strlen(header), body, body_len);
        response->len = strlen(header) + body_len;
    } else {
        fprintf(stderr, "http.c - retrieve_file_contents() - malloc failed for *response->content\n");
    }
    free(header);
    free(body);
    fwrite(response->content, 1, response->len, stdout);
    printf("%ld\n%ld\n", response->len, body_len);
    return response;
}
char *get_response_200_headers(char *content_type) {
    char *header = malloc(HTTP_RESPONSE_200_HEADER_MAX_LEN * sizeof(char));

    snprintf(header, HTTP_RESPONSE_200_HEADER_MAX_LEN, HTTP_RESPONSE_200_HEADER, content_type);
    return header;
}
char *parse_requested_path(char *req, int reqLen) {
    char *path = malloc(MAXIMUM_PATH_STR_LEN * sizeof(char));
    char request_path[MAXIMUM_REQUEST_PATH_STR_LEN];
    sscanf(req, "GET %s", request_path);

    snprintf(path, MAXIMUM_PATH_STR_LEN, "%s%s", web_root_path, request_path);
    return path;
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
char *retrieve_file_contents_text(FILE *f, long *file_len) {
    char *content;

    // Analyze the length of the file
    fseek(f, 0, SEEK_END);
    *file_len = ftell(f);
    fseek(f, 0, SEEK_SET);

    content = malloc(*file_len * sizeof(char));
    if (content) {
        fread(content, sizeof(char), *file_len, f);
    } else {
        fprintf(stderr, "http.c - retrieve_file_contents_text() - malloc failed for *content...\n");
        return NULL;
    }
    return content;
}
unsigned char *retrieve_file_contents_binary(FILE *f, long *file_len) {
    unsigned char *content;

    // Analyze the length of the file
    fseek(f, 0, SEEK_END);
    *file_len = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    content = malloc(*file_len * sizeof(unsigned char));
    if (content) {
        fread(content, sizeof(unsigned char), *file_len, f);
    } else {
        fprintf(stderr, "http.c - retrieve_file_contents_binary() - malloc failed for *content...\n");
        return NULL;
    }
    return content;
}
http_response *http_response_constructor() {
    http_response *response = malloc(sizeof(http_response));
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