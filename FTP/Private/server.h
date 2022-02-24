#ifndef COEUS_FTP_SERVER_H
#define COEUS_FTP_SERVER_H

#include <stdbool.h>

#include "config.h"
#include "connection.h"
#include "socket.h"
#include "thread.h"
#include "worker.h"

struct server
{
    config_t config;

    socket_t * socket_list;

    size_t socket_list_size;

    worker_t ** worker_list;

    size_t worker_list_size;

};

typedef struct server server_t;

bool server_init(server_t * server, int argc, char ** argv);

void server_term(server_t * server);

bool server_run(server_t * server);

#endif // COEUS_FTP_SERVER_H