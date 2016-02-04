#ifndef COEUS_MAIL_SMTP_WORKER_HPP
#define COEUS_MAIL_SMTP_WORKER_HPP

#include <string>
#include <asio.hpp>

using std::string;
using asio::ip::tcp;

class SMTPServer;

class SMTPWorker
{
public:

    SMTPWorker() = delete;
    SMTPWorker(const SMTPWorker&) = delete;
    SMTPWorker& operator=(const SMTPWorker&) = delete;

    SMTPWorker(SMTPServer* pServer, tcp::socket sock);
    ~SMTPWorker() = default;

    void Run();

private:

    void HandleRequest();

    SMTPServer* mp_Server;

    tcp::socket m_Sock;

};

#endif // COEUS_MAIL_SMTP_WORKER_HPP
