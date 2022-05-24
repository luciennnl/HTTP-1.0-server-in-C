#include "../header/socket_worker.h"

void *socket_worker(void *arg) {
    worker_args *args = arg;
    
    char *request;
    // Use void pointer for both char and unsigned char
    void *response = NULL;
    long response_len;
    if (args->response_func != NULL) {
        request = args->read_func(args->connfd);
        response = args->response_func(&response_len, request);
        // If response is empty, something went wrong, don't send
        if (response != NULL) {
            send(args->connfd, response, response_len, 0);
            // Free the response string
            free(response);
        }
        free(request);
    }
    close(args->connfd);
    free(args);
    return NULL;
}
worker_args *worker_args_constructor(int connfd, char *(*read_func)(int), void *(*response_func)(long*, char*)) {
    worker_args *new_worker_args = malloc(sizeof(worker_args));
    if (!new_worker_args) {
        fprintf(stderr, "socket_worker.c - worker_args_constructor()- malloc failed for worker_args\n");
        exit(ERROR_STATUS_CODE);
    }
    new_worker_args->response_func = response_func;
    new_worker_args->read_func = read_func;
    new_worker_args->connfd = connfd;
    return new_worker_args;
}