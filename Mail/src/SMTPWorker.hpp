#ifndef COEUS_MAIL_SMTP_WORKER_HPP
#define COEUS_MAIL_SMTP_WORKER_HPP

#include <string>
#include <map>
#include <asio.hpp>

using std::string;
using std::map;
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
    void Stop();

private:

    void CommandMail(string data);
    void CommandRecipient(string data);
    void CommandData(string data);

    void SendMessage(string msg);

    SMTPServer* mp_Server;

    tcp::socket m_Sock;

    bool m_Running;

    map<string, std::function<void(string)>> m_CommandHandlers;

};

#endif // COEUS_MAIL_SMTP_WORKER_HPP
