#ifndef COEUS_COMMON_CONFIG_PARSER_HPP
#define COEUS_COMMON_CONFIG_PARSER_HPP

#include <map>
#include <string>

using std::map;
using std::string;

bool ParseConfig(const string& filename, map<string, string>& config);

#endif // COEUS_COMMON_CONFIG_PARSER_HPP
