#include "../header/http.h"
char *http_get(uint64_t *response_len, char* req, int reqlen) {
    FILE *f;
    char *path = parse_requested_path(req, reqlen);
    f = fopen(path, "r");
    free(path);
    if (f == NULL) {
        return generate_response_404(response_len);
    }
    return retrieve_file_contents(f, response_len);
}
char *parse_requested_path(char *req, int reqLen) {
    char *path = malloc(MAXIMUM_PATH_STR_LEN * sizeof(char));
    char request_path[MAXIMUM_REQUEST_PATH_STR_LEN];
    sscanf(req, "GET %s", request_path);

    snprintf(path, MAXIMUM_PATH_STR_LEN, "%s%s", web_root_path, request_path);
    return path;
}
char *generate_response_404(uint64_t *response_len) {
    char *response = malloc(sizeof(HTTP_RESPONSE_404));
    *response_len = sizeof(HTTP_RESPONSE_404);
    strcpy(response, HTTP_RESPONSE_404);
    return response;
}
char *retrieve_file_contents(FILE *f, uint64_t *response_len) {
    long length;
    char *response;
    char *response_body;

    // Analyze the length of the file
    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    response_body = malloc((length * sizeof(char)));
    if (response_body) {
        fread(response_body, sizeof(char), length, f);

        // Add extra space for the header
        // TODO: might need to do sizeof(HTTP_RESPONSE_200) - 1 for null terminator
        response = malloc(sizeof(HTTP_RESPONSE_200) + (length * sizeof(char)));
        if (response) {

            //Combine header and body
            snprintf(response, (sizeof(HTTP_RESPONSE_200) + (length * sizeof(char))), "%s%s", HTTP_RESPONSE_200, response_body);
        } else {
            fprintf(stderr, "http.c - retrieve_file_contents() - malloc failed for *response...\n");
            free(response_body);
            return NULL;
        }
    } else {
        fprintf(stderr, "http.c - retrieve_file_contents() - malloc failed for *response_body...\n");
        return NULL;
    }
    free(response_body);
    *response_len = sizeof(HTTP_RESPONSE_200) + (length * sizeof(char));
    return response;
}