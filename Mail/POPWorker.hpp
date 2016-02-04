#ifndef COEUS_MAIL_POP_WORKER_HPP
#define COEUS_MAIL_POP_WORKER_HPP

#include <string>
#include <asio.hpp>

using std::string;
using asio::ip::tcp;

class POPServer;

class POPWorker
{
public:

    POPWorker() = delete;
    POPWorker(const POPWorker&) = delete;
    POPWorker& operator=(const POPWorker&) = delete;

    POPWorker(POPServer* pServer, tcp::socket sock);
    ~POPWorker() = default;

    void Run();

private:

    void HandleRequest();

    POPServer* mp_Server;

    tcp::socket m_Sock;

};

#endif // COEUS_MAIL_POP_WORKER_HPP
