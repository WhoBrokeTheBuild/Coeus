#include "SMTPServer.hpp"
#include "POPServer.hpp"
#include "IMAPServer.hpp"
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

    std::thread smtpThread([&](){
        SMTPServer* pServer = new SMTPServer(config);
        pServer->Run();
        delete pServer;
    });

    std::thread popThread([&](){
        POPServer* pServer = new POPServer(config);
        pServer->Run();
        delete pServer;
    });

    std::thread imapThread([&](){
        IMAPServer* pServer = new IMAPServer(config);
        pServer->Run();
        delete pServer;
    });

    smtpThread.join();
    popThread.join();
    imapThread.join();

    return 0;
}
