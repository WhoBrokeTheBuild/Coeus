#ifndef COEUS_FTP_CONNECTION_H
#define COEUS_FTP_CONNECTION_H

#include "socket.h"

typedef struct server server_t;

enum data_type
{
    DATA_TYPE_ASCII = 0,
    DATA_TYPE_EBCDIC,
    DATA_TYPE_IMAGE,
    DATA_TYPE_LOCAL,
};

typedef enum data_type data_type_t;

struct connection
{
    server_t * server;

    socket_t socket;

    struct sockaddr_in6 address;
    
    socket_t data_socket;

    struct sockaddr_in6 data_address;

    char command_buffer[4096];

    size_t command_buffer_offset;

    data_type_t data_type;
};

typedef struct connection connection_t;

void connection_init(connection_t * conn, server_t * server);

void connection_term(connection_t * conn);

void connection_start(connection_t * conn, socket_t socket, struct sockaddr_in6 * address);

void connection_handle_input(connection_t * conn);

#endif // COEUS_FTP_CONNECTION_H