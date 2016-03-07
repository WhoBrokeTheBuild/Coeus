#ifndef COEUS_HTTP_CONNECTION_HPP
#define COEUS_HTTP_CONNECTION_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <functional>
#include <asio.hpp>

#include <Event.hpp>
#include "ConnInfo.hpp"

using std::string;
using std::map;
using std::vector;
using std::function;
using asio::ip::tcp;

class HTTPServer;

class HTTPConnection
{
public:

    HTTPConnection() = delete;
    HTTPConnection(const HTTPConnection&) = delete;
    HTTPConnection& operator=(const HTTPConnection&) = delete;

    HTTPConnection(HTTPServer* pServer, tcp::socket sock);
    ~HTTPConnection() = default;

    static void AddFileTypeHandler(const string& mimeType, function<string(const string&, ConnInfo*)> callback);

    void Run();

    static Event<ConnInfo*> OnReadRequest;
    static Event<ConnInfo*> OnParseRequest;
    static Event<ConnInfo*> OnBeginPrepareResponse;

private:

    static map<string, function<string(const string&, ConnInfo*)>> s_FileTypeHandlers;

    void HandleRequest();

    void ReadRequest();
    void WriteBufferResponse(const string& buf);
    void WriteFileResponse(std::ifstream& file);

    void LogRequest();

    string GetDirectoryListHTML(const string& path, const string& realPath);

    HTTPServer* mp_Server;
    tcp::socket m_Sock;

    ConnInfo m_ConnInfo;
};

#endif // COEUS_HTTP_CONNECTION_HPP
