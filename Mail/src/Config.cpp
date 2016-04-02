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

	const auto& smtpPortIt = config.find("smtpport");
	if (smtpPortIt != config.cend())
	{
		std::stringstream ss(smtpPortIt->second);
		ss >> m_SMTPPort;
	}

	const auto& popPortIt = config.find("popport");
	if (popPortIt != config.cend())
	{
		std::stringstream ss(popPortIt->second);
		ss >> m_POPPort;
	}

	const auto& imapPortIt = config.find("imapport");
	if (imapPortIt != config.cend())
	{
		std::stringstream ss(imapPortIt->second);
		ss >> m_IMAPPort;
	}

    return true;
}
