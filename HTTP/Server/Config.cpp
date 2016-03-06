#include "Config.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Linker.hpp"
#include <Functions.hpp>
#include <ConfigParser.hpp>

using std::ifstream;

Config::Config()
{
	AddDirectiveHandler("MimeFile",
		[this](const string& file)
		{
			LoadMime(file);
		});

	AddDirectiveHandler("Indexes",
		[this](const string& indexes)
		{
			vector<string> pieces;
			std::istringstream ss(indexes);
			std::copy(std::istream_iterator<string>(ss),
				std::istream_iterator<string>(),
				std::back_inserter(pieces));
			m_Indexes = vector<string>(pieces.begin() + 1, pieces.end());
		});

	AddDirectiveHandler("Port",
		[this](const string& port)
		{
			std::stringstream ss(port);
			ss >> m_Port;
		});

	AddDirectiveHandler("ContentRoot",
		[this](const string& contentRoot)
		{
			m_ContentRoot = contentRoot;
		});

	AddDirectiveHandler("ErrorPage404",
		[this](const string& page404)
		{
			m_404File = page404;
		});

	AddDirectiveHandler("LoadModule",
		[this](const string& module)
		{
			LoadModule(module);
		});
}

void Config::AddDirectiveHandler(const string& directive, std::function<void(const string&)> callback)
{
	string lowerDirective = directive;
	std::transform(lowerDirective.begin(), lowerDirective.end(), lowerDirective.begin(), ::tolower);
	m_DirectiveHandlers[lowerDirective] = callback;
}

bool Config::Load(const string& filename)
{
	std::ifstream file(filename);

	if (!file)
	{
		return false;
	}

	string line;
	while (!file.eof())
	{
		getline(file, line);

		if (line.length() != 0 && line.front() == '#')
			continue;

        std::stringstream ss(CleanWhitespace(line));

        string cmd;
        ss >> cmd;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        string data;
        getline(ss, data);
		data = StringTrim(data);

        if (cmd.front() == '!')
        {
            if (cmd == "!include")
            {
				string dir = Dirname(filename);
				string subFilename = dir + "/" + data;
                if (!Load(subFilename))
				{
					fprintf(stderr, "Cannot open config file %s\n", subFilename.c_str());
					return false;
				}
            }
        }
        else
        {
            if (!data.empty())
            {
				auto callbackIt = m_DirectiveHandlers.find(cmd);
				if (callbackIt != m_DirectiveHandlers.end())
				{
					callbackIt->second(data);
				}
            }
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
