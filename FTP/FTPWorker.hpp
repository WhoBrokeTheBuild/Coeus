#ifndef COEUS_FTP_WORKER_HPP
#define COEUS_FTP_WORKER_HPP

#include "Constants.hpp"
#include <asio.hpp>
#include <string>
#include <map>

using asio::ip::tcp;
using std::string;
using std::map;

class FTPServer;

class FTPWorker
{
public:

    FTPWorker(const FTPWorker&) = delete;
    FTPWorker& operator=(const FTPWorker&) = delete;

    FTPWorker(FTPServer* pServer, tcp::socket sock);
    ~FTPWorker() = default;

    void Run();
    void Stop();

private:

    void CommandType(string data);
    void CommandPort(string data);
    void CommandList(string data);
    void CommandChangeDir(string data);
    void CommandReturn(string data);
    void CommandStore(string data);
    void CommandDelete(string data);
    void CommandMakeDir(string data);
    void CommandRemoveDir(string data);

    void SendMessage(string msg);
    void LogRequest(string msg);
    void LogResponse(string msg);

    FTPServer* mp_Server;

    unsigned long long m_ConnId;

    bool m_Running;

    tcp::socket m_Sock;
    tcp::socket m_DataSock;

    tcp::endpoint m_DataEndpoint;

    string m_Pwd = "/";

    TransferMode m_TransMode = TransferMode::ASCII;

    map<string, std::function<void(string data)>> m_CommandHandlers;

};

#endif // COEUS_FTP_WORKER_HPP
