#include "Config.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Functions.hpp"

using std::ifstream;

Config::Config()
{

}

bool Config::Load(const string& filename)
{
	ifstream file(filename);

	if (!file)
	{
		fprintf(stderr, "Error: Cannot open config file %s\n", filename.c_str());
		return false;
	}

	string line;
	while (!file.eof())
	{
		getline(file, line);

		if (line.length() != 0 && line[0] == '#')
			continue;

		string newLine = CleanWhitespace(line);

        std::transform(newLine.begin(), newLine.end(), newLine.begin(), ::tolower);

		vector<string> pieces;
        std::istringstream iss(newLine);
        std::copy(std::istream_iterator<string>(iss),
            std::istream_iterator<string>(),
            std::back_inserter(pieces));

        if (pieces.size() < 2)
            continue;

        const string& command = pieces[0];
        if (command == "mimefile")
        {
            const string& mimeConfigFile = pieces[1];
            LoadMime(mimeConfigFile);
        }
        else if (command == "indexes")
        {
            m_Indexes = vector<string>(pieces.begin() + 1, pieces.end());
        }
        else if (command == "port")
        {
            std::stringstream ss;
            ss << pieces[1];
            ss >> m_Port;
        }
        else if (command == "contentroot")
        {
            m_ContentRoot = pieces[1];
        }
        else if (command == "errorpage404")
        {
            m_404File = pieces[1];
        }

    }

    return true;
}

bool Config::LoadMime(const string& filename)
{
	ifstream file(filename);

	if (!file)
		return false;

	string line;
	while (!file.eof())
	{
		getline(file, line);

		if (line.length() != 0 && line[0] == '#')
			continue;

		const string& newLine = CleanWhitespace(line);

		vector<string> pieces;
        std::istringstream iss(line);
        std::copy(std::istream_iterator<string>(iss),
            std::istream_iterator<string>(),
            std::back_inserter(pieces));

		if (pieces.size() < 2)
			continue;

		const string& mimeType = pieces[0];
		m_MimeTypes.emplace(mimeType, vector<string>());
		vector<string>& extensions = m_MimeTypes[mimeType];

		for (unsigned int i = 1; i < pieces.size(); ++i)
			extensions.push_back(pieces[i]);
	}

	file.close();

    return true;
}

string Config::GetMimeType(const string& extension) const
{
	for (const auto& it : m_MimeTypes)
	{
		const auto& extensionList = it.second;
		if (std::find(extensionList.begin(), extensionList.end(), extension) != extensionList.end())
		{
			return it.first;
		}
	}

	return "application/octet-stream";
}
