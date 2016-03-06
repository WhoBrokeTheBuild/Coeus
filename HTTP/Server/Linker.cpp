#include "Linker.hpp"

#include <dlfcn.h>
#include <iostream>

bool LoadModule(const string& module)
{
    void* mod = dlopen(module.c_str(), RTLD_NOW);
    if (!mod)
    {
        fprintf(stderr, "Error loading module '%s': %s\n", module.c_str(), dlerror());
        return false;
    }

    dlerror();

    typedef void(*mod_func_t)();

    mod_func_t initFunc = (mod_func_t)dlsym(mod, "InitModule");
    if (!initFunc)
    {
        fprintf(stderr, "Error loading InitModule function from '%s'\n", module.c_str());
        dlclose(mod);
        return false;
    }

    initFunc();
    return true;
}
