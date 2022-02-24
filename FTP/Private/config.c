#include "config.h"
#include "util.h"
#include "log.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

int _config_get_somaxconn()
{
#if defined(_WIN32)

    return SOMAXCONN;

#else

    int somaxconn = SOMAXCONN;

    FILE * file = fopen("/proc/sys/net/core/somaxconn", "rt");
    if (!file) {
        return somaxconn;
    }

    fscanf(file, "%d", &somaxconn);
    fclose(file);

    return somaxconn;

#endif
}

void _config_push_listen(config_t * config, struct sockaddr * addr, size_t size)
{
    for (size_t i = 0; i < config->listen_list_size; ++i) {
        if (memcmp(&config->listen_list[i], addr, size) == 0) {
            return;
        }
    }

    size_t new_size = (config->listen_list_size + 1) * sizeof(struct sockaddr_in6);
    struct sockaddr_in6 * new_listen_list = (struct sockaddr_in6 *)realloc(config->listen_list, new_size);
    if (!new_listen_list) {
        log_error("Out of memory");
        exit(1);
    }

    config->listen_list = new_listen_list;

    memcpy(&config->listen_list[config->listen_list_size], addr, size);
    ++config->listen_list_size;
}

bool _config_parse_listen(config_t * config, const char * line)
{
    int result;

    char * colon = strrchr(line, ':');
    if (colon == NULL) {
        unsigned short port = 0;
        result = sscanf(line, "%hu", &port);
        if (result != 1) {
            log_error("Failed to parse 'Listen %s'", line);
            return false;
        }

        struct sockaddr_in addr4 = {
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr = { 0 },
        };
        _config_push_listen(config, (struct sockaddr *)&addr4, sizeof(struct sockaddr_in));

        struct sockaddr_in6 addr6 = {
            .sin6_family = AF_INET6,
            .sin6_port = htons(port),
            .sin6_addr = { 0 },
        };
        _config_push_listen(config,(struct sockaddr *)&addr6, sizeof(struct sockaddr_in6));
    }
    else {
        struct sockaddr_in6 address = {
            .sin6_port = htons(21),
        };
        
        if (!socket_address_from_string(&address, line)) {
            log_error("Failed to parse 'Listen %s'", line);
            return false;
        }

        _config_push_listen(config,(struct sockaddr *)&address, sizeof(struct sockaddr_in6));
    }

    return true;
}

bool _config_valid(config_t * config)
{
    // TODO: Improve
    if (config->listen_list_size > FD_SETSIZE) {
        log_error("Unable to listen to %zu addresses, limit is %d",
            config->listen_list_size,
            FD_SETSIZE
        );
        return false;
    }

    if (config->connections_per_worker > FD_SETSIZE) {
        log_error("Unable to handle %d connections per worker, limit is %d",
            config->connections_per_worker,
            FD_SETSIZE
        );
        return false;
    }

    int somaxconn = _config_get_somaxconn();
    
    if (config->listen_backlog <= 0) {
        config->listen_backlog = somaxconn;
    }
    else if (config->listen_backlog > somaxconn) {
        config->listen_backlog = somaxconn;

        log_error("Limiting listen backlog to %d",
            somaxconn
        );
    }

    return true;
}

bool config_load(config_t * config, const char * filename)
{
    memset(config, 0, sizeof(config_t));

    // TODO: Move?
    config->access_log_filename = strdup("cftp.log");
    config->error_log_filename = strdup("cftp.log");
    config->listen_list = NULL;
    config->listen_backlog = 8;
    config->min_workers = 2;
    config->max_workers = 10;
    config->connections_per_worker = 10;
    config->file_root = strdup("ftp_root");

    log_access("Reading config from '%s'", filename);

    FILE * file = fopen(filename, "rt");
    if (!file) {
        log_error("Failed to open '%s': %s",
            filename,
            strerror(errno)
        );
        return false;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), file)) {
        char * line = strip_string(buffer);
        
        if (!line || line[0] == '\0' || line[0] == '#') {
            continue;
        }

        char * key = strtok(line, " \t");
        char * value = strip_string(strtok(NULL, ""));
        
        if (strncmp(key, "AccessLog", sizeof("AccessLog")) == 0) {
            free(config->access_log_filename);
            config->access_log_filename = strdup(value);
        }
        else if (strncmp(key, "ErrorLog", sizeof("ErrorLog")) == 0) {
            free(config->error_log_filename);
            config->error_log_filename = strdup(value);
        }
        else if (strncmp(key, "Listen", sizeof("Listen")) == 0) {
            _config_parse_listen(config, value);
        }
        else if (strncmp(key, "ListenBacklog", sizeof("ListenBacklog")) == 0) {
            sscanf(value, "%d", &config->listen_backlog);
        }
        else if (strncmp(key, "MinWorkers", sizeof("MinWorkers")) == 0) {
            sscanf(value, "%d", &config->min_workers);
        }
        else if (strncmp(key, "MaxWorkers", sizeof("MaxWorkers")) == 0) {
            sscanf(value, "%d", &config->max_workers);
        }
        else if (strncmp(key, "ConnectionsPerWorker", sizeof("ConnectionsPerWorker")) == 0) {
            sscanf(value, "%d", &config->connections_per_worker);
        }
        else if (strncmp(key, "FileRoot", sizeof("FileRoot")) == 0) {
            free(config->file_root);
            config->file_root = strdup(value);
        }
    }

    fclose(file);

    if (!config->listen_list) {
        _config_parse_listen(config, "21");
    }

    if (!_config_valid(config)) {
        return false;
    }

    return true;
}

void config_free(config_t * config)
{
    free(config->access_log_filename);
    config->access_log_filename = NULL;
    
    free(config->error_log_filename);
    config->error_log_filename = NULL;
    
    free(config->listen_list);
    config->listen_list = NULL;
    config->listen_list_size = 0;
    
    free(config->file_root);
    config->file_root = NULL;
}
