#include "Config.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Functions.hpp"

using std::ifstream;

Config::Config() :
	m_Port(),
	m_RootDir()
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

        if (command == "port")
        {
            std::stringstream ss;
            ss << pieces[1];
            ss >> m_Port;
        }
        else if (command == "rootdir")
        {
			m_RootDir = pieces[1];
        }

    }


    return true;
}
