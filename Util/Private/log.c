#include "log.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

FILE * _access_log = NULL;

FILE * _error_log = NULL;

bool log_init(const char * access_filename, const char * error_filename)
{
    log_term();

    _access_log = fopen(access_filename, "wt");
    if (!_access_log) {
        return false;
    }
    
    if (strcmp(access_filename, error_filename) == 0) {
        _error_log = _access_log;
    }
    else {
        _error_log = fopen(error_filename, "wt");
        if (!_error_log) {
            return false;
        }
    }

    return true;
}

void log_term()
{
    if (_access_log == _error_log) {
        _error_log = NULL;
    }

    if (_access_log) {
        fclose(_access_log);
        _access_log = NULL;
    }

    if (_error_log) {
        fclose(_error_log);
        _error_log = NULL;
    }
}

void log_access(const char * format, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    fprintf(stdout, "%s\n", buffer);
    fflush(stdout);

    if (_access_log) {
        fprintf(_access_log, "%s\n", buffer);
        fflush(_access_log);
    }
}

void log_error(const char * format, ...)
{
    char buffer[1024];
    
    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    fprintf(stderr, "%s\n", buffer);
    fflush(stderr);

    if (_error_log) {
        fprintf(_error_log, "%s\n", buffer);
        fflush(_error_log);
    }
}