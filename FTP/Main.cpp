#include "FTPServer.hpp"
#include <getopt.h>
#include <cstdlib>
#include <cstdio>
#include <thread>

void usage()
{

}

int main(int argc, char** argv)
{
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

    std::thread ftpThread([&](){
        FTPServer* pServer = new FTPServer(config);
        pServer->Run();
        delete pServer;
    });

    ftpThread.join();

    return 0;
}
