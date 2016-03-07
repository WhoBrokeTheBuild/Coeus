#ifndef HTTP_MOD_HEADER_HPP
#define HTTP_MOD_HEADER_HPP

#include <string>
#include <map>
#include <Linker.hpp>

using std::string;
using std::map;

extern "C" void InitModule();

extern map<string, string> gExtraHeaders;

#endif // HTTP_MOD_HEADER_HPP
