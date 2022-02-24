#include "socket.h"

bool network_init()
{
    #if defined(_WIN32)

        WSADATA wsa_data;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (result != 0) {
            fprintf(stderr,
                "WSAStartup() failed: %d\n",
                result
            );
            return false;
        }

    #endif

    return true;
}

void network_term()
{
    #if defined(_WIN32)

        WSACleanup();

    #endif
}

void socket_set_non_blocking(socket_t sock)
{
    #if defined(_WIN32)

        DWORD flag = true;
        ioctlsocket(sock, FIONBIO, &flag);

    #else

        int flags = fcntl(sock, F_GETFL, 0);
        if (flags == -1) {
            return;
        }

        flags |= O_NONBLOCK;
        fcntl(sock, F_SETFL, flags);

    #endif
}

bool socket_address_from_string(struct sockaddr_in6 * sockaddr, const char * string)
{
    char buffer[INET6_ADDRSTRLEN];
    strncpy(buffer, string, sizeof(buffer));

    char * pch = buffer;
    char * colon = strrchr(pch, ':');

    sockaddr->sin6_family = AF_INET;
    if (pch[0] == '[') {
        sockaddr->sin6_family = AF_INET6;
        ++pch;

        char * closing_bracket = strchr(pch, ']');
        if (!closing_bracket) {
            log_error("Invalid IPv6 address, missing closing ]");
            return false;
        }

        *closing_bracket = '\0';
        if (colon < closing_bracket) {
            colon = NULL;
        }
    }

    if (colon) {
        *colon = '\0';
        
        unsigned short port;
        if (sscanf(colon + 1, "%hu", &port) == 1) {
            sockaddr->sin6_port = htons(port);
        }
    }

    void * addr = (
        sockaddr->sin6_family == AF_INET6 
        ? (void *)&sockaddr->sin6_addr
        : (void *)&((struct sockaddr_in *)sockaddr)->sin_addr
    );

    int result = inet_pton(sockaddr->sin6_family, pch, addr);
    if (result == 0) {
        log_error(
            "inet_pton() failed: '%s' is not a valid address",
            buffer
        );
        return false;
    }
    else if (result < 0) {
        log_error(
            "inet_pton() failed: %s",
            strerror(errno)
        );
        return false;
    }

    return true;
}

bool socket_address_to_string(char * buffer, size_t size, struct sockaddr_in6 * sockaddr)
{
    if (sockaddr->sin6_family == AF_INET6) {
        buffer[0] = '[';
        ++buffer;
        --size;
    }

    void * addr = (
        sockaddr->sin6_family == AF_INET6 
        ? (void *)&sockaddr->sin6_addr
        : (void *)&((struct sockaddr_in *)sockaddr)->sin_addr
    );

    if (!inet_ntop(sockaddr->sin6_family, addr, buffer, size)) {
        log_error(
            "inet_ntop() failed: %s",
            strerror(errno)
        );
        return false;
    }

    size_t length = strlen(buffer);

    if (sockaddr->sin6_family == AF_INET6) {
        strncat(buffer + length, "]", size - length);
        ++length;
    }

    if (sockaddr->sin6_port > 0) {
        snprintf(
            buffer + length,
            size - length,
            ":%hu",
            ntohs(sockaddr->sin6_port)
        );
    }

    return true;
}

char * socket_get_error()
{
    #if defined(_WIN32)

        static char buffer[1024];

        FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            WSAGetLastError(),
            0,
            buffer,
            sizeof(buffer),
            NULL
        );

        return buffer;

    #else

        return strerror(errno);

    #endif
}