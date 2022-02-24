#ifndef COEUS_UTIL_UTIL_H
#define COEUS_UTIL_UTIL_H

#include <string.h>
#include <ctype.h>

#if defined(_WIN32)

#include <Windows.h>

#else

#include <unistd.h>

#endif

inline void sleep_ms(int ms)
{
#if defined(_WIN32)

    Sleep(ms);

#else

    usleep(ms * 1000);

#endif
}

char * strip_string(char * str);

#endif // COEUS_UTIL_UTIL_H