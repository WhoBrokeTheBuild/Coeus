#ifndef COEUS_FTP_CONFIG_H
#define COEUS_FTP_CONFIG_H

#include "socket.h"

#include <stdbool.h>

struct config
{
    // AccessLog
    char * access_log_filename;
    
    // ErrorLog
    char * error_log_filename;

    // Listen
    struct sockaddr_in6 * listen_list;

    size_t listen_list_size;

    // ListenBacklog
    int listen_backlog;
    
    // MinWorkers
    int min_workers;

    // MaxWorkers
    int max_workers;

    // ConnectionsPerWorker
    int connections_per_worker;

    // FileRoot
    char * file_root;

};

typedef struct config config_t;

bool config_load(config_t * config, const char * filename);

void config_free(config_t * config);

#endif // COEUS_FTP_CONFIG_H