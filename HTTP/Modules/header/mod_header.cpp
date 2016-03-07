#include "mod_header.hpp"

#include <cstdio>
#include <sstream>
#include <Config.hpp>
#include <HTTPConnection.hpp>
#include <Functions.hpp>

map<string, string> gExtraHeaders;

extern "C" void InitModule()
{
    Config::AddDirectiveHandler("SetHeader",
        [](const string& header)
        {
            std::stringstream ss(CleanWhitespace(header));

            string key;
            ss >> key;

            string data;
            getline(ss, data);
        	data = StringTrim(data);

            gExtraHeaders[key] = data;
        });

    HTTPConnection::OnBeginPrepareResponse.AddListener(
        [](ConnInfo* pConnInfo)
        {
            for (const auto& kv : gExtraHeaders)
            {
                pConnInfo->ResponseHeaders.emplace(kv.first, kv.second);
            }
        });
}
