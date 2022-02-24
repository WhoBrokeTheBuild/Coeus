#ifndef COEUS_WORKER_H
#define COEUS_WORKER_H

#include "thread.h"
#include "connection.h"

typedef struct server server_t;

struct worker
{
    server_t * server;

    thread_t thread;
    
    bool running;

    connection_t * connection_list;

    size_t connection_list_size;

    thread_mutex_t connection_list_mutex;
};

typedef struct worker worker_t;

void worker_init(worker_t * worker, server_t * server);

void worker_term(worker_t * worker);

#endif // COEUS_WORKER_H