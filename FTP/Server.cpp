#include "Server.hpp"

#include "Worker.hpp"
#include <thread>

Server::Server(const string& configFile) :
    m_Config(),
    m_IoSvc()
{
    m_Config.Load(configFile);
}

Server::~Server()
{

}

void Server::Run()
{
    if (m_Config.GetPort() == 0)
    {
        fprintf(stderr, "Error: No port specified\n");
        return;
    }

    tcp::acceptor a(m_IoSvc, tcp::endpoint(tcp::v4(), m_Config.GetPort()));
    printf("FTP Server running on port %d\n", m_Config.GetPort());

    for (;;)
    {
        tcp::socket sock(m_IoSvc);
        a.accept(sock);
        std::thread(
            [this](tcp::socket sock)
            {
                Worker w(this, std::move(sock));
                w.Run();
            },
        std::move(sock)).detach();
    }
}
