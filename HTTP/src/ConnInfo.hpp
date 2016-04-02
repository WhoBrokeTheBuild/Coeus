#ifndef COEUS_HTTP_CONN_INFO
#define COEUS_HTTP_CONN_INFO

#include <string>
#include <map>
#include <functional>

using std::string;
using std::map;
using std::function;

struct ConnInfo
{
public:

    ConnInfo() = default;
    ~ConnInfo() = default;
    ConnInfo& operator=(const ConnInfo&) = default;

    // Request
    string Verb;
    string Protocol;
    string Path;
    map<string, string> RequestHeaders;

    // Response
    unsigned short ResponseCode;
    unsigned short ResponseSize;
    string ResponseMessage;
    map<string, string> ResponseHeaders;
};

#endif // COEUS_HTTP_CONN_INFO
