#include "../header/server.h"

int main(int argc, char *argv[]) {
    config *cfg = read_input(argc, argv);

    printf("%d, %d, %s\n", cfg->protocol, cfg->port, cfg->path_to_web_root);

    return 0;
}

config *read_input(int argc, char *argv[]) {
    if (argc != EXPECTED_INPUT_CNT + 1) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected 3 arguments but got %d\n", argc);
        exit(1);
    }
    
    config *res = malloc(sizeof(config));
    
    int protocol = atoi(argv[PROTOCOL_ARG_NO]);
    if (protocol != IPV4_NO && protocol != IPV6_NO) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected either 4 (IPV4) or 6 (IPV6) for the first argument\n");
        exit(1);
    }
    res->protocol = protocol;
    res->port = atoi(argv[PORT_ARG_NO]);
    res->path_to_web_root = argv[PATH_TO_WEB_ROOT_ARG_NO];

    return res;
}