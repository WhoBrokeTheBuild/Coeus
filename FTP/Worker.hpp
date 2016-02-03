#ifndef COEUS_FTP_WORKER_HPP
#define COEUS_FTP_WORKER_HPP

#include "Constants.hpp"
#include <asio.hpp>
#include <string>

using asio::ip::tcp;
using std::string;

class Server;

class Worker
{
public:

    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    Worker(Server* pServer, tcp::socket sock);
    ~Worker();

    void Run();
    void Stop();

private:

    bool SendMessage(string msg);

    Server* mp_Server;

    tcp::socket m_Sock;
    tcp::socket m_DataSock;

    tcp::endpoint m_DataEndpoint;

    string m_Pwd = "/";

    TransferMode m_TransMode = TransferMode::ASCII;

};

#endif // COEUS_FTP_WORKER_HPP
