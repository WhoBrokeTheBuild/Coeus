#include "util.h"

char * strip_string(char * str)
{
    if (!str) {
        return NULL;
    }
    
    while (isspace(*str)) {
        ++str;
    }

    size_t length = strlen(str);
    while (length > 0 && isspace(str[length - 1])) {
        str[length - 1] = '\0';
        --length;
    }

    return str;
}