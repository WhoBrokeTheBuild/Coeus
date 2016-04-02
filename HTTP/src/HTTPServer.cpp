#include "HTTPServer.hpp"

#include "HTTPConnection.hpp"
#include <thread>
#include <cstdio>

HTTPServer::HTTPServer(const Config& config) :
    m_Config(config),
    m_IoSvc()
{ }

void HTTPServer::Run()
{
    if (m_Config.GetPort() == 0)
    {
        fprintf(stderr, "Error: No port specified\n");
        return;
    }

    tcp::acceptor a(m_IoSvc, tcp::endpoint(tcp::v4(), m_Config.GetPort()));
    printf("HTTP Server running on port %d\n", m_Config.GetPort());

    for (;;)
    {
        tcp::socket sock(m_IoSvc);
        a.accept(sock);
        std::thread(
            [this](tcp::socket sock)
            {
                HTTPConnection conn(this, std::move(sock));
                conn.Run();
            },
        std::move(sock)).detach();
    }
}
