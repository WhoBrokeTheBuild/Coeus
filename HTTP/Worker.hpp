#ifndef COEUS_HTTP_WORKER_HPP
#define COEUS_HTTP_WORKER_HPP

#include <string>
#include <map>
#include <vector>
#include <asio.hpp>

using std::string;
using std::map;
using std::vector;
using asio::ip::tcp;

class Server;

class Worker
{
public:

    Worker() = delete;
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    Worker(Server* pServer, tcp::socket sock);
    ~Worker();

    void Run();

private:

    void GetHeader();
    void HandleRequest();
    void SendDirectoryList(const string& path, const string& realPath);

    Server* mp_Server;

    tcp::socket m_Sock;

    string m_Method;
    string m_Path;
    string m_Protocol;

    map<string, string> m_Headers;

};

#endif // COEUS_HTTP_WORKER_HPP
