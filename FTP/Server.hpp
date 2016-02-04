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

    inline const Config* GetConfig() const { return &m_Config; }
    inline asio::io_service& GetIoService() { return m_IoSvc; }

    inline unsigned long long GetNextConnId() { return ++m_NextConnId; }

private:

    Config m_Config;
    asio::io_service m_IoSvc;

    unsigned long long m_NextConnId = 0;

};

#endif // COEUS_FTP_SERVER_HPP
