#include "../header/server.h"

int main(int argc, char *argv[]) {
    config *cfg = read_input(argc, argv);

    web_root_path = cfg->path_to_web_root;
    socket_info *socket = socket_init(cfg->protocol_no, cfg->port);
    
    socket_listen(socket->fd, socket->addr);

    socket_handle_messages(socket->fd, 10, &http_get_string_adaptor);
    close(socket->fd);

    socket_free(socket);
    free(cfg);
    return 0;
}
config *read_input(int argc, char *argv[]) {
    if (argc != EXPECTED_INPUT_CNT + 1) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected 3 arguments but got %d\n", (argc-1));
        exit(0);
    }
    
    config *res = malloc(sizeof(config));
    
    int protocol_no = atoi(argv[PROTOCOL_NO_ARG_NO]);
    if (protocol_no != IPV4 && protocol_no != IPV6) {
        fprintf(stderr, "server.c - read_input() - Failed to read input: Expected either 4 (IPV4) or 6 (IPV6) for the first argument\n");
        exit(0);
    }
    res->protocol_no = protocol_no;
    res->port = argv[PORT_ARG_NO];
    res->path_to_web_root = argv[PATH_TO_WEB_ROOT_ARG_NO];

    return res;
}