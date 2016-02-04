#include "HTTPServer.hpp"

#include "CSL.hpp"
#include <getopt.h>
#include <cstdlib>
#include <cstdio>

void usage()
{

}

int main(int argc, char** argv)
{
    printf("Running CSL Version: %s\n", CSL::GetVersionString());

    string config;

    struct option longopts[] = {
        { "config",    required_argument, NULL, 'f' },
        { 0, 0, 0, 0 }
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "f:", longopts, NULL)) != -1)
    {
        switch (opt) {
            case 'f':
                config = string(optarg);
                break;
        }
    }

    if (config.empty())
    {
        fprintf(stderr, "Error: Please specify a config file\n");
        usage();
        return 1;
    }

    HTTPServer* pServer = new HTTPServer(config);
    pServer->Run();
    delete pServer;

    return 0;
}
