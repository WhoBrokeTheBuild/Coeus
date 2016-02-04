#include "Functions.hpp"

#include <cstdio>
#include <sstream>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

void Pause()
{
    printf("Press enter to continue...\n");
    getchar();
}

std::streamsize GetFileSize(std::ifstream& file)
{
	std::streampos fsize = file.tellg();
	file.seekg(0, std::ios::end);
	fsize = file.tellg() - fsize;
	file.seekg(0, std::ios::beg);

	return fsize;
}

string CleanWhitespace(string str)
{
    str = StringTrim(str);

    // Reduce Middle Whitespace

	string newStr;
	int whitespaceStart = -1;
    bool inString = false;
	for (unsigned int i = 0; i < str.length(); ++i)
	{
        if (str[i] == '"')
        {
            inString = !inString;
        }

        if (inString)
        {
			newStr += str[i];
            continue;
        }

		if (str[i] == ' ' || str[i] == '\t')
		{
			if (whitespaceStart == -1)
			{
				newStr += ' ';
				whitespaceStart = i;
			}

			if (i != str.length() - 1 && str[i + 1] != ' ' && str[i + 1] != '\t')
				whitespaceStart = -1;
		}
		else
			newStr += str[i];
	}

	return newStr;
}

string StringTrim(string str)
{
    if (str.length() == 0) return str;

    if (str.length() == 1)
    {
        if (std::isspace(str[0]))
            return "";
        else
            return str;
    }

    // Trim Left

    int endOfWhitespace = -1;
    for (unsigned int i = 0; i < str.length(); ++i)
    {
        if (!std::isspace(str[i]))
        {
            endOfWhitespace = i;
            break;
        }
    }

    if (endOfWhitespace != -1)
    {
        str = str.substr(endOfWhitespace);
    }

    // Trim Right

    int startOfWhitespace = -1;
    for (unsigned int i = str.length() - 1; i >= 0; --i)
    {
        if (!std::isspace(str[i]))
        {
            startOfWhitespace = i + 1;
            break;
        }
    }

    if (startOfWhitespace != -1)
    {
        str = str.substr(0, startOfWhitespace);
    }

    return str;
}

vector<string> StringSplit(const string& str, const string& delim, int limit /*= -1*/)
{
    vector<string> stringParts;

	unsigned int lastEnd = 0;
	for (unsigned int i = 0; i < str.length(); ++i)
	{
		bool check = true;
		for (unsigned int j = 0; j < delim.length(); ++j)
		{
			if (str[i + j] != delim[j])
			{
				check = false;
				break;
			}
		}

		if (check)
		{
			if (limit != -1 && stringParts.size() >= (unsigned)limit - 1)
				break;

			stringParts.push_back(str.substr(lastEnd, i - lastEnd));
			lastEnd = i + delim.length();
		}
	}

	if (lastEnd < str.length())
	{
		stringParts.push_back(str.substr(lastEnd));
	}

	return stringParts;
}

string Basename(const string& path)
{
	const string& splitString = (path.find("\\") != string::npos ? "\\" : "/");

	vector<string> parts = StringSplit(path, splitString);

	if (parts.empty())
		return "";

	return parts.back();
}

bool MakeDir(const string& path)
{
    int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if (status)
        return false;

    return true;
}

time_t GetFileLastModified(const string& filename)
{
    struct tm* clock;
    struct stat attrib;
    stat(filename.c_str(), &attrib);
    clock = gmtime(&(attrib.st_mtime));

    return mktime(clock);
}

vector<DirectoryEntry> GetDirectoryList(const string& path)
{
    vector<DirectoryEntry> entries;

	DIR *dir;
	struct dirent *ent;

	dir = opendir(path.c_str());
	if (dir != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
            DirectoryEntry::EntryType type;
			switch (ent->d_type)
			{
			case DT_REG:

				type = DirectoryEntry::EntryType::File;

				break;
			case DT_DIR:

				type = DirectoryEntry::EntryType::Directory;

				break;
			}

            entries.push_back(DirectoryEntry(ent->d_ino, ent->d_name, type));
		}

		closedir(dir);
	}

    return entries;
}
