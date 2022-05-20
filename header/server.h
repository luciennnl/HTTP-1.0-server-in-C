#ifndef SERVER_H

#define SERVER_H

#define IMPLEMENTS_IPV6
#define MULTITHREADED

#define _POSIX_C_SOURCE 200112L
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "./socket.h"
#include "./http.h"

#define EXPECTED_INPUT_CNT 3
#define PROTOCOL_NO_ARG_NO 1
#define PORT_ARG_NO 2
#define PATH_TO_WEB_ROOT_ARG_NO 3
char* web_root_path;
struct config {
    protocol_number protocol_no;
    char *port;
    char *path_to_web_root;
};

typedef struct config config;

/**
 * @brief Function to read command line arguments into a config struct
 *        INFO: Will exit on unexpected or malformed input.
 * @param argc The amount of arguments
 * @param argv An array of strings containing the arguments
 * @return config* The constructed config struct
 */
config *read_input(int argc, char *argv[]);

/**
 * @brief Function to free a config struct
 * 
 * @param cfg A pointer to the config struct to free
 */
void config_free(config *cfg);
#endif