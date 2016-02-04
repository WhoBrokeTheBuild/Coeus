#ifndef COEUS_HTTP_WORKER_HPP
#define COEUS_HTTP_WORKER_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <asio.hpp>

using std::string;
using std::map;
using std::vector;
using asio::ip::tcp;

class HTTPServer;

class HTTPWorker
{
public:

    HTTPWorker() = delete;
    HTTPWorker(const HTTPWorker&) = delete;
    HTTPWorker& operator=(const HTTPWorker&) = delete;

    HTTPWorker(HTTPServer* pServer, tcp::socket sock);
    ~HTTPWorker();

    void Run();

private:

    void HandleRequest();

    void ReadRequestHeader();
    void WriteBufferResponse(const string& buf);
    void WriteFileResponse(std::ifstream& file);

    void MethodGetHandler(const bool& sendBody = true);
    void MethodPostHandler();

    void LogRequest();

    string GetDirectoryListHTML(const string& path, const string& realPath);

    HTTPServer* mp_Server;

    tcp::socket m_Sock;

    string m_Method;
    string m_Path;
    string m_Protocol;

    map<string, string> m_Headers;

    map<string, std::function<void()>> m_Handlers;

    unsigned short m_RespCode;
    unsigned short m_RespSize;
    string m_RespMsg;
    map<string, string> m_RespHeaders;
    string m_RespBody;

};

#endif // COEUS_HTTP_WORKER_HPP
