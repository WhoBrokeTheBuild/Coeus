#include "worker.h"
#include "server.h"
#include "connection.h"

void * _worker_thread_function(void * arg)
{
    worker_t * worker = (worker_t *)arg;
    server_t * server = worker->server;

    log_access("Worker starting");

    FD_SET fds;
    while (worker->running) {
        FD_ZERO(&fds);

        thread_mutex_lock(&worker->connection_list_mutex);

        socket_t max_fd = 0;
        for (size_t i = 0; i < worker->connection_list_size; ++i) {
            connection_t * conn = &worker->connection_list[i];

            if (socket_valid(conn->socket)) {
                FD_SET(conn->socket, &fds);
                
                if (conn->socket > max_fd) {
                    max_fd = conn->socket + 1;
                }
            }
        }

        thread_mutex_unlock(&worker->connection_list_mutex);

        if (max_fd == 0) {
            continue;
        }

        struct timeval select_timeout = {
            .tv_sec = 1,
            .tv_usec = 0,
        };
        
        int result = select(max_fd, &fds, NULL, NULL, &select_timeout);
        if (result == 0) {
            continue;
        }
        else if (result < 0) {
            log_error("select() failed: %s", socket_get_error());
            return false;
        }
        
        for (size_t i = 0; i < worker->connection_list_size; ++i) {
            connection_t * conn = &worker->connection_list[i];

            if (socket_valid(conn->socket) && FD_ISSET(conn->socket, &fds)) {
                connection_handle_input(conn);
            }
        }
    }
    
    log_access("Worker stopping");
    return NULL;
}

void worker_init(worker_t * worker, server_t * server)
{
    worker->server = server;
    worker->running = true;
    
    worker->connection_list_size = server->config.connections_per_worker;
    worker->connection_list = (connection_t *)calloc(
        worker->connection_list_size,
        sizeof(connection_t)
    );

    if (!worker->connection_list) {
        log_error("Out of memory");
        exit(1);
    }

    for (size_t i = 0; i < worker->connection_list_size; ++i) {
        connection_init(&worker->connection_list[i], server);
    }

    thread_mutex_init(&worker->connection_list_mutex);

    thread_create(&worker->thread, _worker_thread_function, worker);
}

void worker_term(worker_t * worker)
{
    worker->server = NULL;
    worker->running = false;
    thread_join(worker->thread);
    
    thread_mutex_term(&worker->connection_list_mutex);
    
    for (size_t i = 0; i < worker->connection_list_size; ++i) {
        connection_term(&worker->connection_list[i]);
    }
}
