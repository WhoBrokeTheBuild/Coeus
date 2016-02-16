#ifndef COEUS_SMTP_SERVER_HPP
#define COEUS_SMTP_SERVER_HPP

#include "Config.hpp"
#include <asio.hpp>

using asio::ip::tcp;

class SMTPServer
{
public:

    SMTPServer(const SMTPServer&) = delete;
    SMTPServer& operator=(const SMTPServer&) = delete;

    SMTPServer(const Config& config);
    ~SMTPServer() = default;

    void Run();

    inline const Config* GetConfig() const { return &m_Config; }

private:

    Config m_Config;
    asio::io_service m_IoSvc;

};

#endif // COEUS_SMTP_SERVER_HPP
