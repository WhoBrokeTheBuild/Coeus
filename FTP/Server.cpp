#include "Server.hpp"

Server::Server(const string& configFile) :
    m_Config(),
    m_IoSvc()
{
    m_Config.Load(configFile);
}

void Server::Run()
{
    if (m_Config.GetCommandPort() == 0)
    {
        fprintf(stderr, "Error: No command port specified\n");
        return;
    }

    if (m_Config.GetDataPort() == 0)
    {
        fprintf(stderr, "Error: No data port specified\n");
        return;
    }

    tcp::acceptor a(m_IoSvc, tcp::endpoint(tcp::v4(), m_Config.GetCommandPort()));
    printf("FTP Server running on command port %d\n", m_Config.GetCommandPort());
}

void Server::HandleConnection(tcp::socket sock)
{

}
