#ifndef COEUS_POP_SERVER_HPP
#define COEUS_POP_SERVER_HPP

#include "Config.hpp"
#include <asio.hpp>

using asio::ip::tcp;

class POPServer
{
public:

    POPServer(const POPServer&) = delete;
    POPServer& operator=(const POPServer&) = delete;

    POPServer(const Config& config);
    ~POPServer() = default;

    void Run();

    inline const Config* GetConfig() const { return &m_Config; }

private:

    Config m_Config;
    asio::io_service m_IoSvc;

};

#endif // COEUS_POP_SERVER_HPP
