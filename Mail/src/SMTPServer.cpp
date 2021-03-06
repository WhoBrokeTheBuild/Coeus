#include "SMTPServer.hpp"

#include "SMTPWorker.hpp"
#include <thread>

SMTPServer::SMTPServer(const Config& config) :
    m_Config(config),
    m_IoSvc()
{ }

void SMTPServer::Run()
{
    if (m_Config.GetSMTPPort() == 0)
    {
        fprintf(stderr, "Error: No port specified\n");
        return;
    }

    tcp::acceptor a(m_IoSvc, tcp::endpoint(tcp::v4(), m_Config.GetSMTPPort()));
    printf("SMTP Server running on port %d\n", m_Config.GetSMTPPort());

    for (;;)
    {
        tcp::socket sock(m_IoSvc);
        a.accept(sock);
        std::thread(
            [this](tcp::socket sock)
            {
                SMTPWorker w(this, std::move(sock));
                w.Run();
            },
        std::move(sock)).detach();
    }
}
