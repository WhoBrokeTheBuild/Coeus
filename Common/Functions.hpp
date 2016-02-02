#ifndef COEUS_FUNCTIONS_HPP
#define COEUS_FUNCTIONS_HPP

#include <string>
#include <fstream>
#include <vector>

using std::string;
using std::vector;

void Pause();

std::streamsize GetFileSize(std::ifstream& file);

string CleanWhitespace(string str);

string StringTrim(string str);

vector<string> StringSplit(const string& str, const string& delim, int limit = -1);

string Basename(const string& path);

#endif // COEUS_FUNCTIONS_HPP
