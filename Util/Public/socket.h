#ifndef COEUS_UTIL_SOCKET_H
#define COEUS_UTIL_SOCKET_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)

    #include <WinSock2.h>
    #include <WS2tcpip.h>

    #pragma comment (lib, "Ws2_32.lib")

    typedef SOCKET socket_t;

#else

    #include <unistd.h>
    #include <sys/socket.h>
    #include <fcntl.h>

    typedef int socket_t;

    #define INVALID_SOCKET (socket_t)(-1)

#endif

bool network_init();

void network_term();

void socket_set_non_blocking(socket_t sock);

bool socket_address_from_string(struct sockaddr_in6 * sockaddr, const char * string);

bool socket_address_to_string(char * buffer, size_t size, struct sockaddr_in6 * address);

char * socket_get_error();

inline void socket_close(socket_t sock)
{
    #if defined(_WIN32)

        closesocket(sock);

    #else

        close(sock);

    #endif
}

inline bool socket_valid(socket_t sock)
{
    #if defined(_WIN32)

        return (sock != INVALID_SOCKET);

    #else

        return (sock >= 0);

    #endif
}


#endif // COEUS_UTIL_SOCKET_H