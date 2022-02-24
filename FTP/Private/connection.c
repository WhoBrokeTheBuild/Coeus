#include "connection.h"
#include "server.h"
#include "util.h"

#include <inttypes.h>

void connection_init(connection_t * conn, server_t * server)
{
    conn->server = server;
    conn->command_buffer[0] = '\0';
    conn->command_buffer[sizeof(conn->command_buffer) - 1] = '\0';
    conn->command_buffer_offset = 0;
    conn->data_type = DATA_TYPE_ASCII;

    conn->socket = INVALID_SOCKET;
    memset(&conn->address, 0, sizeof(struct sockaddr_in6));
    
    conn->data_socket = INVALID_SOCKET;
    memset(&conn->data_address, 0, sizeof(struct sockaddr_in6));
}

void connection_term(connection_t * conn)
{
    if (conn->data_socket != INVALID_SOCKET) {
        socket_close(conn->data_socket);
        conn->data_socket = INVALID_SOCKET;
    }

    if (conn->socket != INVALID_SOCKET) {
        socket_close(conn->socket);
        conn->socket = INVALID_SOCKET;
    }
}

bool _connection_read(connection_t * conn)
{
    if (conn->command_buffer_offset >= sizeof(conn->command_buffer) - 1) {
        // TODO: Reconsider
        return false;
    }

    int bytes_read = recv(
        conn->socket,
        conn->command_buffer + conn->command_buffer_offset,
        sizeof(conn->command_buffer) - conn->command_buffer_offset - 1,
        0
    );

    if (bytes_read < 0) {
        log_error("recv() failed: %s", socket_get_error());

        return false;
    }

    conn->command_buffer_offset += bytes_read;
    conn->command_buffer[conn->command_buffer_offset] = '\0';

    return true;
}

void _connection_write(connection_t * conn, char * format, ...)
{
    va_list args;
    va_start(args, format);

    static char buffer[4096];
    int chars_used = vsnprintf(buffer, sizeof(buffer), format, args);
    strncat(buffer, "\r\n", sizeof(buffer) - chars_used - 1);

    int bytes_written = send(
        conn->socket,
        buffer,
        strlen(buffer),
        0
    );
    
    if (bytes_written < 0) {
        connection_term(conn);
    }
}

bool _connection_connect_data_socket(connection_t * conn, int family, char * address, unsigned short port)
{
    char buffer[INET6_ADDRSTRLEN];
    if (family == 1) {
        snprintf(buffer, sizeof(buffer), "%s:%hu", address, port);
    }
    else if (family == 2) {
        snprintf(buffer, sizeof(buffer), "[%s]:%hu", address, port);
    }
    else {
        _connection_write(conn, "522 Unknown network protocol");
        return false;
    }

    if (!socket_address_from_string(&conn->data_address, buffer)) {
        return false;
    }

    if (socket_valid(conn->data_socket)) {
        socket_close(conn->data_socket);
    }

    conn->data_socket = socket(conn->data_address.sin6_family, SOCK_STREAM, 0);
    if (!socket_valid(conn->data_socket)) {
        return false;
    }

    int addrlen = sizeof(struct sockaddr_in6);
    int result = connect(conn->data_socket, (struct sockaddr *)&conn->data_address, addrlen);
    if (result < 0) {
        log_error("connect() failed: %s", strerror(errno));
        return false;
    }

    _connection_write(conn, "200 Data connection opened on %s", address);

    return true;
}

void connection_start(connection_t * conn, socket_t socket, struct sockaddr_in6 * address)
{
    conn->socket = socket;
    memcpy(&conn->address, &address, sizeof(conn->address));

    _connection_write(conn, "220 Service ready for new user.");
}

void connection_handle_input(connection_t * conn)
{
    _connection_read(conn);

    char * crlf = strstr(conn->command_buffer, "\r\n");
    while (crlf) {
        *crlf = '\0';

        char * line = strip_string(conn->command_buffer);

        log_access("%s", line);

        // TODO: Possibly simplify, all commands will be 4 characters
        char * key = strtok(line, " \t");
        char * value = strip_string(strtok(NULL, ""));

        if (strcmp(key, "USER") == 0) {
            _connection_write(conn, "200 Command okay.");
        }
        else if (strcmp(key, "PASS") == 0) {
            
        }
        else if (strcmp(key, "SYST") == 0) {
            _connection_write(conn, "215 UNIX Type: L8");
        }
        else if (strcmp(key, "FEAT") == 0) {
            _connection_write(conn, "211-Extensions supported.");

            _connection_write(conn, "211 END.");
        }
        else if (strcmp(key, "PWD") == 0) {
            _connection_write(conn, "257 /");
        }
        else if (strcmp(key, "TYPE") == 0) {
            if (value[0] == 'A') {
                conn->data_type = DATA_TYPE_ASCII;
                _connection_write(conn, "200 Data type changed to ASCII.");
            }
            else if (value[0] == 'E') {
                conn->data_type = DATA_TYPE_EBCDIC;
                _connection_write(conn, "200 Data type changed to EBCDIC.");
            }
            else if (value[0] == 'I') {
                conn->data_type = DATA_TYPE_IMAGE;
                _connection_write(conn, "200 Data type changed to IMAGE.");
            }
            else if (value[0] == 'L') {

            }
            else {
                _connection_write(conn, "500 Unrecognized data type.");
            }
        }
        else if (strcmp(key, "MODE") == 0) {
            
        }
        else if (strcmp(key, "PORT") == 0) {
            int octets[6];
            int result = sscanf(value,
                "%d,%d,%d,%d,%d,%d",
                &octets[0],
                &octets[1],
                &octets[2],
                &octets[3],
                &octets[4],
                &octets[5]
            );

            if (result == 6) {
                unsigned short port = (octets[4] * 256) + octets[5];
                char address[INET_ADDRSTRLEN];
                snprintf(address, sizeof(address),
                    "%d.%d.%d.%d",
                    octets[0],
                    octets[1],
                    octets[2],
                    octets[3]
                );

                _connection_connect_data_socket(conn, AF_INET, address, port);
            }
            else {
                _connection_write(conn, "500 Syntax error.");
            }
        }
        else if (strcmp(key, "EPRT") == 0) {
            char delim[] = { value[0], '\0' };
            char * str_family = strtok(value, delim);
            char * address = strtok(NULL, delim);
            char * str_port = strtok(NULL, delim);

            int family = 0;
            int result = sscanf(str_family, "%d", &family);

            if (family > 0) {
                unsigned short port;
                result += sscanf(str_port, "%hu", &port);

                if (address && result == 2) {
                    _connection_connect_data_socket(conn, family, address, port);
                }
                else {
                    _connection_write(conn, "500 Syntax error.");
                }
            }
        }
        else if (strcmp(key, "PASV") == 0) {
            
        }
        else if (strcmp(key, "EPSV") == 0) {
            
        }
        else if (strcmp(key, "LIST") == 0) {
            if (value) {

            }
            else {
                _connection_write(conn, "125 Data connection already open; transfer starting.");
                
                const char * test = 
                    "-rw-rw-rw- 1 user group 0 Jan 1 1970 test1.txt\r\n"
                    "-rw-rw-rw- 1 user group 0 Jan 1 1970 test2.txt\r\n"
                    "-rw-rw-rw- 1 user group 0 Jan 1 1970 test3.txt\r\n";
                send(conn->data_socket, test, strlen(test), 0);

                // _connection_write(conn, "250 Requested file action okay, completed");

                _connection_write(conn, "226 Transfer complete.");
                socket_close(conn->data_socket);
                conn->data_socket = INVALID_SOCKET;
            }
        }
        else if (strcmp(key, "STRU") == 0) {
            
        }
        else if (strcmp(key, "RETR") == 0) {
            _connection_write(conn, "125 Data connection already open; transfer starting.");
                
            const char * test = "Fuck off\n";
            send(conn->data_socket, test, strlen(test), 0);

            // _connection_write(conn, "250 Requested file action okay, completed");

            _connection_write(conn, "226 Transfer complete.");
            socket_close(conn->data_socket);
            conn->data_socket = INVALID_SOCKET;
        }
        else if (strcmp(key, "STOR") == 0) {
            
        }
        else if (strcmp(key, "QUIT") == 0) {
            
        }
        else if (strcmp(key, "NOOP") == 0) {
            
        }
        else {
            _connection_write(conn, "500 Unrecognized command.");
        }

        crlf += 2;
        conn->command_buffer_offset -= (crlf - conn->command_buffer);
        memmove(conn->command_buffer, crlf, conn->command_buffer_offset);

        crlf = strstr(conn->command_buffer, "\r\n");
    }
}