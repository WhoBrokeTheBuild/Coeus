#ifndef COEUS_FTP_SERVER_HPP
#define COEUS_FTP_SERVER_HPP

#include "Config.hpp"
#include <asio.hpp>

using asio::ip::tcp;

class Server
{
public:

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    Server(const string& configFile);
    ~Server();

    void Run();
    void HandleConnection(tcp::socket sock);

private:

    Config m_Config;
    asio::io_service m_IoSvc;

};

#endif // COEUS_FTP_SERVER_HPP
