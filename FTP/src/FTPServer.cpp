#include "FTPServer.hpp"

#include "FTPConnection.hpp"
#include <thread>

FTPServer::FTPServer(const FTPConfig& config) :
    m_Config(config),
    m_IoSvc()
{ }

void FTPServer::Run()
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
                FTPConnection conn(&m_Config, m_IoSvc, std::move(sock), GetNextConnId());
                conn.Run();
            },
        std::move(sock)).detach();
    }
}
