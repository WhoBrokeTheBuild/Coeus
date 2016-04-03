#ifndef COEUS_FTP_WORKER_HPP
#define COEUS_FTP_WORKER_HPP

#include "Constants.hpp"
#include <asio.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>

using asio::ip::tcp;
using std::string;
using std::unordered_map;

class FTPConfig;

class FTPConnection
{
public:

    FTPConnection(const FTPConnection&) = delete;
    FTPConnection& operator=(const FTPConnection&) = delete;

    FTPConnection(FTPConfig* pConfig, asio::io_service& ioSvc, tcp::socket sock, const uint64_t& connId);
    ~FTPConnection() = default;

    void Run();
    void Stop();

private:

    void InitCommandHandlers();

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

    bool m_Running;

    FTPConfig* mp_Config;

    tcp::socket m_Sock;
    tcp::socket m_DataSock;
    tcp::endpoint m_DataEndpoint;

    unordered_map<string, std::function<void(string data)>> m_CommandHandlers;

    uint64_t m_ConnId;
    string m_Pwd = "/";
    TransferMode m_TransMode = TransferMode::ASCII;

};

#endif // COEUS_FTP_WORKER_HPP
