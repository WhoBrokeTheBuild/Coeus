#include "ConfigParser.hpp"

#include "Functions.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

bool ParseConfig(const string& filename, map<string, string>& config)
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
            if (cmd == "include")
            {
				string dir = Dirname(filename);
                if (!ParseConfig(dir + "/" + data, config));
            }
        }
        else
        {
            if (!data.empty())
            {
                config.emplace(cmd, data);
            }
        }
    }

    return true;
}
