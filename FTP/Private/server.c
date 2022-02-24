#include "server.h"
#include "util.h"
#include "worker.h"
#include "log.h"

#include <string.h>
#include <signal.h>

bool server_init(server_t * server, int argc, char ** argv)
{
    memset(server, 0, sizeof(server_t));

    if (!config_load(&server->config, "cftp.cfg")) {
        return false;
    }

    if (!log_init(server->config.access_log_filename, server->config.error_log_filename)) {
        return false;
    }

    if (!network_init()) {
        return false;
    }

    server->socket_list_size = server->config.listen_list_size;
    server->socket_list = (socket_t *)calloc(server->socket_list_size, sizeof(socket_t));
    if (!server->socket_list) {
        log_error("Out of memory");
        exit(1);
    }
    
    char buffer[INET6_ADDRSTRLEN];
    for (size_t i = 0; i < server->socket_list_size; ++i) {
        struct sockaddr_in6 * address = &server->config.listen_list[i];

        server->socket_list[i] = socket(address->sin6_family, SOCK_STREAM, 0);

        if (!socket_valid(server->socket_list[i])) {
            log_error("socket() failed: %s", socket_get_error());
            return false;
        }

        socket_address_to_string(buffer, sizeof(buffer), address);
        log_access("Listening on %s", buffer);

        int addrlen = sizeof(struct sockaddr_in6);
        int result = bind(server->socket_list[i], (struct sockaddr *)address, addrlen);
        if (result < 0) {
            log_error("bind() failed: %s", socket_get_error());
            return false;
        }
    }

    server->worker_list_size = server->config.min_workers;
    server->worker_list = (worker_t **)calloc(server->config.min_workers, sizeof(worker_t *));
    if (!server->worker_list) {
        log_error("Out of memory");
        exit(1);
    }

    for (size_t i = 0; i < server->worker_list_size; ++i) {
        server->worker_list[i] = (worker_t *)calloc(1, sizeof(worker_t));
    }

    return true;
}

void server_term(server_t * server)
{
    for (size_t i = 0; i < server->worker_list_size; ++i) {
        free(server->worker_list[i]);
    }
    free(server->worker_list);
    server->worker_list = NULL;
    server->worker_list_size = 0;

    for (size_t i = 0; i < server->socket_list_size; ++i) {
        socket_close(server->socket_list[i]);
    }

    free(server->socket_list);
    server->socket_list = NULL;
    server->socket_list_size = 0;

    network_term();

    log_term();
    
    config_free(&server->config);
}

bool _server_running = false;

void _server_handle_signal(int signal)
{
    _server_running = false;
}

bool server_run(server_t * server)
{
    log_access("Running Coeus FTP server");
    
    _server_running = true;
    signal(SIGINT, _server_handle_signal);
    
    for (size_t i = 0; i < server->socket_list_size; ++i) {
        int result = listen(server->socket_list[i], server->config.listen_backlog);
        if (result < 0) {
            log_error("listen() failed: %s", socket_get_error());
            return false;
        }
    }
    
    for (size_t i = 0; i < server->worker_list_size; ++i) {
        worker_init(server->worker_list[i], server);
    }

    socket_t client_socket;
    struct sockaddr_in6 client_address;
    char buffer[INET6_ADDRSTRLEN];

    fd_set fds;
    while (_server_running) {
        FD_ZERO(&fds);

        socket_t max_fd = 0;
        for (size_t i = 0; i < server->socket_list_size; ++i) {
            FD_SET(server->socket_list[i], &fds);

            if (server->socket_list[i] > max_fd) {
                max_fd = server->socket_list[i] + 1;
            }
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

        for (size_t i = 0; i < server->socket_list_size; ++i) {
            if (FD_ISSET(server->socket_list[i], &fds)) {
                socklen_t client_addrlen = sizeof(client_address);
                client_socket = accept(server->socket_list[i], (struct sockaddr *)&client_address, &client_addrlen);

                if (!socket_valid(client_socket)) {
                    log_error("accept() failed: %s", socket_get_error());
                    return false;
                }

                socket_set_non_blocking(client_socket);

                if (!socket_address_to_string(buffer, sizeof(buffer), &client_address)) {
                    socket_close(client_socket);
                    continue;
                }

                log_access("Accepted connection from %s", buffer);

                bool worker_found = false;

                for (size_t w = 0; w < server->worker_list_size; ++w) {
                    worker_t * worker = server->worker_list[w];

                    thread_mutex_lock(&worker->connection_list_mutex);

                    for (size_t c = 0; c < server->config.connections_per_worker; ++c) {
                        connection_t * conn = &worker->connection_list[c];

                        if (!socket_valid(conn->socket)) {
                            connection_start(conn, client_socket, &client_address);
                            
                            worker_found = true;
                            break;
                        }
                    }

                    thread_mutex_unlock(&worker->connection_list_mutex);

                    if (worker_found) {
                        break;
                    }
                }

                if (!worker_found) {
                    if (server->worker_list_size < server->config.max_workers) {
                        log_access("Allocating new worker");

                        size_t new_size = (server->worker_list_size + 1) * sizeof(worker_t *);
                        worker_t ** new_worker_list = (worker_t **)realloc(server->worker_list, new_size);
                        if (!new_worker_list) {
                            log_error("Out of memory");
                            exit(1);
                        }

                        server->worker_list = new_worker_list;

                        worker_t * new_worker = (worker_t *)calloc(1, sizeof(worker_t));
                        server->worker_list[server->worker_list_size] = new_worker;
                        ++server->worker_list_size;

                        worker_init(new_worker, server);
                        
                        thread_mutex_lock(&new_worker->connection_list_mutex);

                        connection_start(&new_worker->connection_list[0], client_socket, &client_address);
                        
                        thread_mutex_unlock(&new_worker->connection_list_mutex);
                    }
                    else {
                        log_error("Unable to accept connection, maximum number of workers reached: %d", server->config.max_workers);

                        socket_close(client_socket);
                    }
                }

                // TODO: scale back down
            }
        }
    }
    
    for (size_t i = 0; i < server->worker_list_size; ++i) {
        worker_term(server->worker_list[i]);
    }

    log_access("Stopping Coeus FTP server");
    fflush(stdout);

    return true;
}

