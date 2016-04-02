#include "FTPServer.hpp"

#include <Platform.hpp>
#include <ArgParser.hpp>

#include <cstdlib>
#include <cstdio>
#include <thread>

void usage()
{
    printf("Usage: CoeusFTP [OPTION]...\n");
    printf("The Coeus FTP Server\n");
    printf("\n");
    printf("  -f, --config               path to config file\n");
    printf("  -d                         run process in background\n");
}

#ifndef COEUS_TESTING

int main(int argc, char** argv)
{
    string configFile;

	bool result = ArgParser({
		Option('f', "config", OptionFlags::RequiredArg,
			[&](string val) { configFile = val; })
	}).Run(argc, argv);

    if (configFile.empty())
    {
        fprintf(stderr, "CoeusFTP: Please specify a config file\n");
        usage();
        return 1;
    }

    FTPConfig config;
    if (!config.Load(configFile))
    {
        fprintf(stderr, "CoeusFTP: Failed to load config file %s\n", configFile.c_str());
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

#endif
