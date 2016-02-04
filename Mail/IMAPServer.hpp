#ifndef COEUS_IMAP_SERVER_HPP
#define COEUS_IMAP_SERVER_HPP

#include "Config.hpp"
#include <asio.hpp>

using asio::ip::tcp;

class IMAPServer
{
public:

    IMAPServer(const IMAPServer&) = delete;
    IMAPServer& operator=(const IMAPServer&) = delete;

    IMAPServer(const Config& config);
    ~IMAPServer() = default;

    void Run();

private:

    Config m_Config;
    asio::io_service m_IoSvc;

};

#endif // COEUS_IMAP_SERVER_HPP
