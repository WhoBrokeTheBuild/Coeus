#ifndef COEUS_HTTP_SERVER
#define COEUS_HTTP_SERVER

#include "Config.hpp"
#include <asio.hpp>

using asio::ip::tcp;

class HTTPServer
{
public:

    HTTPServer(const HTTPServer&) = delete;
    HTTPServer& operator=(const HTTPServer&) = delete;

    HTTPServer(const Config& config);
    ~HTTPServer() = default;

    void Run();

    inline const Config* GetConfig() const { return &m_Config; }

private:

    Config m_Config;
    asio::io_service m_IoSvc;

};

#endif // COEUS_HTTP_SERVER
