#ifndef COEUS_UTIL_LOG_H
#define COEUS_UTIL_LOG_H

#include <stdbool.h>

bool log_init(const char * access_filename, const char * error_filename);

void log_term();

void log_access(const char * format, ...);

void log_error(const char * format, ...);

#endif // COEUS_UTIL_LOG_H