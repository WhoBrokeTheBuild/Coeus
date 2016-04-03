#include "FTPServer.hpp"

#include "FTPConnection.hpp"
#include <thread>

FTPServer::FTPServer(const FTPConfig& config) :
    m_Config(config),
    m_IoSvc(),
    mp_Acc(nullptr)
{ }

void FTPServer::Run()
{
    if (m_Config.GetPort() == 0)
    {
        fprintf(stderr, "Error: No port specified\n");
        return;
    }

    mp_Acc.reset(new tcp::acceptor(m_IoSvc, tcp::endpoint(tcp::v4(), m_Config.GetPort())));
    printf("FTP Server running on port %d\n", m_Config.GetPort());

    m_IoSvc.dispatch([this](){
        Accept();
    });
    m_IoSvc.run();
}

void FTPServer::Stop()
{
    m_IoSvc.stop();
}

void FTPServer::Accept()
{
    tcp::socket sock(m_IoSvc);
    mp_Acc->accept(sock);
    std::thread(
        [this](tcp::socket sock)
        {
            FTPConnection conn(&m_Config, m_IoSvc, std::move(sock), GetNextConnId());
            conn.Run();
        },
    std::move(sock)).detach();

    m_IoSvc.dispatch([this](){
        Accept();
    });
}
