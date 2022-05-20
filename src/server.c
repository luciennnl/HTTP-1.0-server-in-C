#include "../header/server.h"

int main(int argc, char *argv[]) {
    config *cfg = read_input(argc, argv);

    web_root_path = cfg->path_to_web_root;
    socket_info *socket = socket_init(cfg->protocol_no, cfg->port);
    
    socket_listen(socket->fd, socket->addr);

    socket_handle_messages(socket->fd, 10, &http_read_adaptor, &http_get_string_adaptor);
    close(socket->fd);

    socket_free(socket);
    config_free(cfg);

    return 0;
}
config *read_input(int argc, char *argv[]) {
    // Check additional arguments is count 3
    if (argc != EXPECTED_INPUT_CNT + 1) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected 3 arguments but got %d\n", (argc-1));
        exit(1);
    }
    
    config *res = malloc(sizeof(config));

    // Parse arguments into config struct
    if (!res) {
        fprintf(stderr, "server.c - read_input()- malloc failed for config\n");
        exit(1);
    }
    int protocol_no = atoi(argv[PROTOCOL_NO_ARG_NO]);
    if (protocol_no != IPV4 && protocol_no != IPV6) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected either 4 (IPV4) or 6 (IPV6) for the first argument\n");
        exit(1);
    }
    res->protocol_no = protocol_no;
    res->port = malloc(strlen(argv[PORT_ARG_NO]) + 1);
    if (!res->port) {
        fprintf(stderr, "server.c - read_input() - Malloc failed for config->port");
        exit(1);
    }
    memcpy(res->port, argv[PORT_ARG_NO], strlen(argv[PORT_ARG_NO]) + 1);

    char* path_to_web_root = argv[PATH_TO_WEB_ROOT_ARG_NO];

    res->path_to_web_root = malloc(strlen(path_to_web_root) + 1);
    if (!res->path_to_web_root) {
        fprintf(stderr, "server.c - read_input() - Malloc failed for config->path_to_web_root");
        exit(1);
    }

    /**
     * @brief Need to perform different string manipulations depending on if the last character is a '/'
     */
    if (path_to_web_root[strlen(path_to_web_root) - 1] == '/') {
        
        memcpy(res->path_to_web_root, path_to_web_root, strlen(path_to_web_root) - 1);
        res->path_to_web_root[strlen(path_to_web_root)] = '\0';
    } else {
        memcpy(res->path_to_web_root, path_to_web_root, strlen(path_to_web_root) + 1);
    }
    return res;
}
void config_free(config *cfg) {
    if (cfg->port) {
        free(cfg->port);
    }
    if (cfg->path_to_web_root) {
        free(cfg->path_to_web_root);
    }
    free(cfg);
}