#include "server.h"

int main(int argc, char ** argv)
{
    server_t server;
    if (server_init(&server, argc, argv)) {
        server_run(&server);
        server_term(&server);
    }
    else {
        return 1;
    }

    return 0;
}