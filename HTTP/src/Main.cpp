#include "HTTPServer.hpp"
#include <CSL.hpp>
#include <getopt.h>
#include <cstdlib>
#include <cstdio>
#include <thread>

void usage()
{

}

int main(int argc, char** argv)
{
    printf("Running CSL Version: %s\n", CSL::GetVersionString());

    string configFile;

    struct option longopts[] = {
        { "config",    required_argument, NULL, 'f' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "f:", longopts, NULL)) != -1)
    {
        switch (opt) {
            case 'f':
                configFile = string(optarg);
                break;
        }
    }

    if (configFile.empty())
    {
        fprintf(stderr, "Error: Please specify a config file\n");
        usage();
        return 1;
    }

    Config config;
    if (!config.Load(configFile))
    {
        fprintf(stderr, "Error: Failed to load config file %s\n", configFile.c_str());
        return 1;
    }

    std::thread httpThread([&](){
        HTTPServer* pServer = new HTTPServer(config);
        pServer->Run();
        delete pServer;
    });

    httpThread.join();

    return 0;
}
