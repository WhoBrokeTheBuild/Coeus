#ifndef COEUS_FTP_SERVER_HPP
#define COEUS_FTP_SERVER_HPP

#include "FTPConfig.hpp"
#include <asio.hpp>
#include <cstdint>

using asio::ip::tcp;

class FTPServer
{
public:

    FTPServer(const FTPServer&) = delete;
    FTPServer& operator=(const FTPServer&) = delete;

    FTPServer(const FTPConfig& config);
    ~FTPServer() = default;

    void Run();

    inline asio::io_service& GetIoService() { return m_IoSvc; }

    inline uint64_t GetNextConnId() { return ++m_NextConnId; }

private:

    FTPConfig m_Config;
    asio::io_service m_IoSvc;

    unsigned long long m_NextConnId = 0;

};

#endif // COEUS_FTP_SERVER_HPP
