#ifndef HTTP_MOD_LUA_HPP
#define HTTP_MOD_LUA_HPP

#include <Linker.hpp>

#include <string>
#include <lua.hpp>

using std::string;

extern "C" void InitModule();
int lua_SetHeader(lua_State* L);
int lua_Output(lua_State* L);

#endif // HTTP_MOD_LUA_HPP
