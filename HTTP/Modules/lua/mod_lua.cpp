#include "mod_lua.hpp"

#include <cstdio>
#include <HTTPConnection.hpp>

string gOutput;
ConnInfo* gConnInfo;

extern "C" void InitModule()
{
    HTTPConnection::AddFileTypeHandler("application/x-lua",
        [](const string& filename, ConnInfo* pConnInfo) -> string
        {
            gConnInfo = pConnInfo;
            gOutput = "";

            lua_State* L = luaL_newstate();

            if (!L)
            {
                fprintf(stderr, "[Lua] Failed to create lua state\n");
                lua_close(L);
                return "";
            }

            luaL_openlibs(L);

            int status = luaL_loadfile(L, filename.c_str());

            if (status)
            {
                fprintf(stderr, "[Lua] %s\n", lua_tostring(L, -1));
                lua_pop(L, 1); // remove error message
                lua_close(L);
                return "";
            }

            lua_register(L, "SetHeader", &lua_SetHeader);
            lua_register(L, "Output", &lua_Output);

            status = lua_pcall(L, 0, LUA_MULTRET, 0);

            if (status)
            {
                fprintf(stderr, "[Lua] %s\n", lua_tostring(L, -1));
                lua_pop(L, 1); // remove error message
                lua_close(L);
                return "";
            }

            lua_close(L);

            gConnInfo->ResponseCode = 200;
            gConnInfo->ResponseMessage = "OK";
            return gOutput;
        });
}

int lua_SetHeader(lua_State* L)
{
    string key = lua_tostring(L, 1);
    string value = lua_tostring(L, 2);

    gConnInfo->ResponseHeaders.emplace(key, value);

    return 0;
}

int lua_Output(lua_State* L)
{
    int argc = lua_gettop(L);
    for (int i = 1; i <= argc; ++i)
    {
        gOutput += lua_tostring(L, i);
    }

    return 0;
}
