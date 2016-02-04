#include "Config.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <Functions.hpp>
#include <ConfigParser.hpp>

using std::ifstream;

Config::Config()
{

}

bool Config::Load(const string& filename)
{
	map<string, string> config;
	if (!ParseConfig(filename, config))
	{
		fprintf(stderr, "Error: Cannot open config file %s\n", filename.c_str());
		return false;
	}

	const auto& mimeFileIt = config.find("mimefile");
	if (mimeFileIt != config.cend())
	{
		LoadMime(mimeFileIt->second);
	}

	const auto& indexesIt = config.find("indexes");
	if (indexesIt != config.cend())
	{
		vector<string> pieces;
		std::istringstream ss(indexesIt->second);
		std::copy(std::istream_iterator<string>(ss),
			std::istream_iterator<string>(),
			std::back_inserter(pieces));
		m_Indexes = vector<string>(pieces.begin() + 1, pieces.end());
	}

	const auto& portIt = config.find("port");
	if (portIt != config.cend())
	{
		std::stringstream ss(portIt->second);
		ss >> m_Port;
	}

	const auto& contentRootIt = config.find("contentroot");
	if (contentRootIt != config.cend())
	{
		m_ContentRoot = contentRootIt->second;
	}

	const auto& errorPage404It = config.find("errorpage404");
    if (errorPage404It != config.cend())
    {
        m_404File = errorPage404It->second;
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
