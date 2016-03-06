#include "mod_header.hpp"

#include <cstdio>
#include <sstream>
#include <Config.hpp>
#include <HTTPWorker.hpp>
#include <Functions.hpp>

map<string, string> gExtraHeaders;

extern "C" void InitModule()
{
    Config::AddDirectiveHandler("SetHeader", HandleSetHeader);
    HTTPWorker::AddBeforeResponseCallback([](HTTPWorker* pWorker){
        for (const auto& kv : gExtraHeaders)
        {
            pWorker->SetRespHeader(kv.first, kv.second);
        }
    });
}

void HandleSetHeader(const string& header)
{
    std::stringstream ss(CleanWhitespace(header));

    string key;
    ss >> key;

    string data;
    getline(ss, data);
	data = StringTrim(data);

    gExtraHeaders[key] = data;
}
