#include "Config.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <Functions.hpp>
#include <ConfigParser.hpp>

using std::ifstream;

Config::Config() :
	m_Port(),
	m_RootDir()
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

	const auto& rootDirIt = config.find("rootdir");
	if (rootDirIt != config.cend())
	{
		m_RootDir = rootDirIt->second;
	}

	const auto& portIt = config.find("port");
	if (portIt != config.cend())
	{
		std::stringstream ss(portIt->second);
		ss >> m_Port;
	}

    return true;
}
