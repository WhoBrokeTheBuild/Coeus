#include "mod_hello.hpp"

#include <cstdio>

extern "C" void InitModule()
{
    printf("[Hello] Hello, World!\n");
}
