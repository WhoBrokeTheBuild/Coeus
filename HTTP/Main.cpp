#include "Server.hpp"

#include <getopt.h>
#include <cstdlib>
#include <cstdio>

void usage()
{

}

int main(int argc, char** argv)
{
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

    Server* pServer = new Server(config);
    pServer->Run();
    delete pServer;

    return 0;
}
