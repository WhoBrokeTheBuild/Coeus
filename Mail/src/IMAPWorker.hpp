#ifndef COEUS_MAIL_IMAP_WORKER_HPP
#define COEUS_MAIL_IMAP_WORKER_HPP

#include <string>
#include <asio.hpp>

using std::string;
using asio::ip::tcp;

class IMAPServer;

class IMAPWorker
{
public:

    IMAPWorker() = delete;
    IMAPWorker(const IMAPWorker&) = delete;
    IMAPWorker& operator=(const IMAPWorker&) = delete;

    IMAPWorker(IMAPServer* pServer, tcp::socket sock);
    ~IMAPWorker() = default;

    void Run();

private:

    void HandleRequest();

    IMAPServer* mp_Server;

    tcp::socket m_Sock;

};

#endif // COEUS_MAIL_IMAP_WORKER_HPP
