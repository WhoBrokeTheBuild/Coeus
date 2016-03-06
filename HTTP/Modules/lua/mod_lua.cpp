#include "mod_lua.hpp"

#include <cstdio>

extern "C" void InitModule()
{
    printf("[Lua] Test\n");
}
