#include "HTTPConnection.hpp"

#include "HTTPServer.hpp"
#include <Functions.hpp>
#include <chrono>
#include <iomanip>

Event<ConnInfo*> HTTPConnection::OnReadRequest;
Event<ConnInfo*> HTTPConnection::OnParseRequest;
Event<ConnInfo*> HTTPConnection::OnBeginPrepareResponse;

map<string, function<string(const string&, ConnInfo*)>> HTTPConnection::s_FileTypeHandlers;

HTTPConnection::HTTPConnection(HTTPServer* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock))
{
}

void HTTPConnection::AddFileTypeHandler(const string& mimeType, function<string(const string&, ConnInfo*)> callback)
{
    s_FileTypeHandlers[mimeType] = callback;
}

void HTTPConnection::Run()
{
    try
    {
        ReadRequest();
    }
    catch (...)
    { }
}

void HTTPConnection::ReadRequest()
{
    asio::streambuf headerBuf;
    asio::read_until(m_Sock, headerBuf, "\r\n\r\n");
    string header = {asio::buffers_begin(headerBuf.data()), asio::buffers_end(headerBuf.data())};

    std::stringstream ss(header);

    string requestLine;
    getline(ss, requestLine);

    vector<string> requestSplit = StringSplit(requestLine, " ", 3);
    std::transform(requestSplit[0].begin(), requestSplit[0].end(), requestSplit[0].begin(), ::toupper);

    m_ConnInfo.Verb = requestSplit[0];
    m_ConnInfo.Path = requestSplit[1];
    m_ConnInfo.Protocol = requestSplit[2];

    string line;
    while (getline(ss, line))
    {
        if(!line.empty() && *line.rbegin() == '\r')
        {
            line.erase(line.length() - 1, 1);
        }

        if (line.empty())
            break;

        vector<string> headerSplit = StringSplit(line, ":", 2);
        if (headerSplit.size() < 2)
            continue;

        m_ConnInfo.RequestHeaders.emplace(StringTrim(headerSplit[0]), StringTrim(headerSplit[1]));
    }

    OnReadRequest.Dispatch(&m_ConnInfo);
    HandleRequest();
}

void HTTPConnection::HandleRequest()
{
    string realPath = mp_Server->GetConfig()->GetContentRoot() + m_ConnInfo.Path;

    std::ifstream file;
    bool directoryListing = false;
    bool fileExists = false;
    if (m_ConnInfo.Path.back() == '/')
    {
        bool indexFound = false;
        const vector<string>& indexes = mp_Server->GetConfig()->GetIndexes();
        for (const auto& indexFile : indexes)
        {
            file.open(realPath + indexFile, std::ios::in | std::ios::binary);

            if (file)
            {
                realPath += indexFile;
                fileExists = true;

                indexFound = true;
                break;
            }
        }

        directoryListing = !indexFound;
    }
    else
    {
        file.open(realPath, std::ios::in | std::ios::binary);
        if (file)
        {
            fileExists = true;
        }
    }

    OnParseRequest.Dispatch(&m_ConnInfo);

    if (directoryListing)
    {
        m_ConnInfo.ResponseCode = 200;
        m_ConnInfo.ResponseMessage = "OK";
        const string& html = GetDirectoryListHTML(m_ConnInfo.Path, realPath);
        WriteBufferResponse(html);
    }
    else if (fileExists)
    {
        vector<string> filenameParts = StringSplit(Basename(realPath), ".");
        const string& ext = filenameParts.back();

        const string& mimeType = mp_Server->GetConfig()->GetMimeType(ext);
        auto callbackIt = s_FileTypeHandlers.find(mimeType);
        if (callbackIt != s_FileTypeHandlers.end())
        {
            const string& buff = callbackIt->second(realPath, &m_ConnInfo);
            WriteBufferResponse(buff);
        }
        else
        {
            m_ConnInfo.ResponseCode = 200;
            m_ConnInfo.ResponseMessage = "OK";
            m_ConnInfo.ResponseHeaders.emplace("Content-Type", mimeType);

            WriteFileResponse(file);
        }
    }
    else
    {
        m_ConnInfo.ResponseCode = 404;
        m_ConnInfo.ResponseMessage = "Not Found";

        file.close();
        file.open(mp_Server->GetConfig()->Get404File());
        m_ConnInfo.ResponseHeaders.emplace("Content-Type", "text/html");

        WriteFileResponse(file);
    }
}

void HTTPConnection::WriteBufferResponse(const string& buff)
{
    OnBeginPrepareResponse.Dispatch(&m_ConnInfo);

    asio::streambuf resp;
    std::ostream respStream(&resp);
    respStream << "HTTP/1.0 " << m_ConnInfo.ResponseCode << " " << m_ConnInfo.ResponseMessage << "\r\n";

    m_ConnInfo.ResponseSize = buff.size();

    std::stringstream ss;
    ss << m_ConnInfo.ResponseSize;
	m_ConnInfo.ResponseHeaders.emplace("Content-Size", ss.str());

    for (const auto& header : m_ConnInfo.ResponseHeaders)
    {
        respStream << header.first << ": " << header.second << "\r\n";
    }

    respStream << "\r\n";
    respStream << buff;

    asio::write(m_Sock, resp);
    LogRequest();
}

void HTTPConnection::WriteFileResponse(std::ifstream& file)
{
    asio::streambuf resp;
    std::ostream respStream(&resp);
    respStream << "HTTP/1.0 " << m_ConnInfo.ResponseCode << " " << m_ConnInfo.ResponseMessage << "\r\n";

    m_ConnInfo.ResponseSize = GetFileSize(file);

    std::stringstream ss;
    ss << m_ConnInfo.ResponseSize;
	m_ConnInfo.ResponseHeaders.emplace("Content-Size", ss.str());

    for (const auto& header : m_ConnInfo.ResponseHeaders)
    {
        respStream << header.first << ": " << header.second << "\r\n";
    }

    respStream << "\r\n";

    asio::write(m_Sock, resp);

    const int TMP_BUFFER_SIZE = 4096;

    char tmp_buffer[TMP_BUFFER_SIZE];
    std::streamsize n;
    do
    {
        file.read(tmp_buffer, TMP_BUFFER_SIZE);
        n = file.gcount();

        if (n == 0)
            break;

        asio::write(m_Sock, asio::buffer(tmp_buffer, n));

        if (!file)
            break;
    } while (n > 0);

    LogRequest();
}

void HTTPConnection::LogRequest()
{
    std::string ipAddr = m_Sock.remote_endpoint().address().to_string();

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%d/%b/%Y:%H:%M:%S %z");

    string referer = "-";
    const auto& refererIt = m_ConnInfo.RequestHeaders.find("Referer");
    if (refererIt != m_ConnInfo.RequestHeaders.cend())
        referer = refererIt->second;

    string userAgent = "-";
    const auto& userAgentIt = m_ConnInfo.RequestHeaders.find("User-Agent");
    if (userAgentIt != m_ConnInfo.RequestHeaders.cend())
        userAgent = userAgentIt->second;

    printf("%s - - [%s] \"%s %s HTTP/1.0\" %d %d \"%s\" \"%s\"\n",
        ipAddr.c_str(), ss.str().c_str(),
        m_ConnInfo.Verb.c_str(), m_ConnInfo.Path.c_str(),
        m_ConnInfo.ResponseCode, m_ConnInfo.ResponseSize,
        referer.c_str(), userAgent.c_str());
}

string HTTPConnection::GetDirectoryListHTML(const string& path, const string& realPath)
{
	std::stringstream output;

	output << "<html><head><title>Directory Listing for " << path << "</title></head>"
		<< "<body><h1>Directory Listing for " << path << "</h1>"
        << "<table><tr><th>Icon</th><th>Name</th><th>Type</th></tr>";

    const vector<DirectoryEntry>& entries = GetDirectoryList(realPath);

    string href;

    string tmpPath = path;
    tmpPath.pop_back();
    size_t lastSlash = tmpPath.rfind('/');
    if (lastSlash != string::npos)
    {
        href = tmpPath.substr(0, lastSlash + 1);
    }

    output << "<tr><td>D</td><td>"
        << "<a href=\"" << href << "\">..</a>"
        << "</td><td>Directory</td></tr>";

    for (const auto& ent : entries)
    {
		if (ent.Filename == "." || ent.Filename == "..")
            continue;

        string type;
        switch (ent.Type)
        {
        case DirectoryEntry::EntryType::File:

            type = "File";

            break;
        case DirectoryEntry::EntryType::Directory:

            type = "Directory";

            break;
        }

        output << "<tr><td>" << type.front() << "</td><td>";

		if (href.length() > 0)
		{
			output << "<a href=\"" << href << "\">" << ent.Filename << "</a>";
		}
		else
        {
			output << ent.Filename;
        }

		output << "</td><td>" << type << "</td></tr>";
    }

	output << "</table></body></html>";
	return output.str();
}
