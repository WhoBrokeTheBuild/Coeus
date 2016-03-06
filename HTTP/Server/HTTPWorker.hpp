#ifndef COEUS_HTTP_WORKER_HPP
#define COEUS_HTTP_WORKER_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <functional>
#include <asio.hpp>

using std::string;
using std::map;
using std::vector;
using std::function;
using asio::ip::tcp;

class HTTPServer;

class HTTPWorker
{
public:

    HTTPWorker() = delete;
    HTTPWorker(const HTTPWorker&) = delete;
    HTTPWorker& operator=(const HTTPWorker&) = delete;

    HTTPWorker(HTTPServer* pServer, tcp::socket sock);
    ~HTTPWorker() = default;

    map<string, string>& GetReqHeaders() { return m_Headers; }
    void SetRespHeader(const string& key, const string& val) { m_RespHeaders[key] = val; }

    static void AddBeforeResponseCallback(function<void(HTTPWorker*)> callback);

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

    map<string, function<void()>> m_Handlers;

    unsigned short m_RespCode;
    unsigned short m_RespSize;
    string m_RespMsg;
    map<string, string> m_RespHeaders;
    string m_RespBody;

    static vector<function<void(HTTPWorker*)>> s_BeforeResponseCallbacks;
};

#endif // COEUS_HTTP_WORKER_HPP
