#ifndef SOCKET_WORKER_H
#define SOCKET_WORKER_H

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "./util.h"

struct worker_args {
    void *(*response_func)(long*, char*);
    char *(*read_func)(int);
    int connfd;
};

typedef struct worker_args worker_args;
/**
 * @brief Function that takes in a pointer to a worker_args struct.
 *        Then reads from the specified connfd in the worker_args based on read_func. This value is passed onto response_func.
 *        Finally, sends a response back to the socket specified in connfd based on what is returned from response_func.
 * @param arg A worker_args containing the information needed to execute the job.
 *        WARN: Despite being a void*, this argument should be provided as a pointer to a worker_args struct.
 * @return NULL when the job has finished
 **/
void *socket_worker(void *arg);

/**
 * @brief Function to create a worker_args struct
 * @param connfd A socket file descriptor
 * @param read_func A function to read from a file descriptor
 * @param response_func A function to return a char* based off what is read
 * @returns worker_args* A pointer to a worker_args struct
 **/
worker_args *worker_args_constructor(int connfd, char *(*read_func)(int), void *(*response_func)(long*, char*));
#endif
