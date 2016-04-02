#ifndef COEUS_FUNCTIONS_HPP
#define COEUS_FUNCTIONS_HPP

#include <string>
#include <fstream>
#include <vector>

using std::string;
using std::vector;

int CalcAverage(const vector<int>& nums);

void Pause();

std::streamsize GetFileSize(std::ifstream& file);

string CleanWhitespace(string str);

string StringTrim(string str);

vector<string> StringSplit(const string& str, const string& delim, int limit = -1);

string Dirname(const string& path);

string Basename(const string& path);

bool MakeDir(const string& path);

bool RemoveDir(const string& path);

bool RemoveFile(const string& path);

bool FileExists(const string& path);

time_t GetFileLastModified(const string& filename);

struct DirectoryEntry
{
    enum EntryType
    {
        Directory,
        File
    };

    inline DirectoryEntry(const unsigned int& id, const string& filename, const EntryType& type) :
        Id(id),
        Filename(filename),
        Type(type)
    { }

    unsigned int Id;
    string Filename;
    EntryType Type;
};

vector<DirectoryEntry> GetDirectoryList(const string& path);

#endif // COEUS_FUNCTIONS_HPP
