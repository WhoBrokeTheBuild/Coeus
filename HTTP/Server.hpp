#ifndef COEUS_HTTP_SERVER
#define COEUS_HTTP_SERVER

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

    inline const Config* GetConfig() const { return &m_Config; }

private:

    Config m_Config;
    asio::io_service m_IoSvc;

};

#endif // COEUS_HTTP_SERVER
