#include "HTTPWorker.hpp"

#include "HTTPServer.hpp"
#include <Functions.hpp>
#include <chrono>
#include <iomanip>

HTTPWorker::HTTPWorker(HTTPServer* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock))
{
    m_Handlers.emplace("get", [this](){ MethodGetHandler(); });
    m_Handlers.emplace("head", [this](){ MethodGetHandler(false); });
    m_Handlers.emplace("post", [this](){ MethodPostHandler(); });
    m_Handlers.emplace("put", [this](){ MethodGetHandler(); });
    m_Handlers.emplace("delete", [this](){ MethodGetHandler(); });
}

HTTPWorker::~HTTPWorker()
{

}

void HTTPWorker::Run()
{
    try
    {
        ReadRequestHeader();
        HandleRequest();
    }
    catch (...)
    { }
}

void HTTPWorker::ReadRequestHeader()
{
    asio::streambuf headerBuf;
    asio::read_until(m_Sock, headerBuf, "\r\n\r\n");
    string header = {asio::buffers_begin(headerBuf.data()), asio::buffers_end(headerBuf.data())};

    std::stringstream ss(header);

    string requestLine;
    getline(ss, requestLine);

    vector<string> requestSplit = StringSplit(requestLine, " ", 3);
    m_Method = requestSplit[0];
    m_Path = requestSplit[1];
    m_Protocol = requestSplit[2];

    std::transform(m_Method.begin(), m_Method.end(), m_Method.begin(), ::tolower);

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

        m_Headers.emplace(StringTrim(headerSplit[0]), StringTrim(headerSplit[1]));
    }
}

void HTTPWorker::HandleRequest()
{
    const auto& handlerIt = m_Handlers.find(m_Method);
    if (handlerIt != m_Handlers.cend())
    {
        handlerIt->second();
    }
}

void HTTPWorker::WriteBufferResponse(const string& buff)
{
    asio::streambuf resp;
    std::ostream respStream(&resp);
    respStream << "HTTP/1.0 " << m_RespCode << " " << m_RespMsg << "\r\n";

    m_RespSize = buff.size();

    std::stringstream ss;
    ss << m_RespSize;
	m_RespHeaders.emplace("Content-Size", ss.str());

    for (const auto& header : m_RespHeaders)
    {
        respStream << header.first << ": " << header.second << "\r\n";
    }

    respStream << "\r\n";
    respStream << buff;

    asio::write(m_Sock, resp);
    LogRequest();
}

void HTTPWorker::WriteFileResponse(std::ifstream& file)
{
    asio::streambuf resp;
    std::ostream respStream(&resp);
    respStream << "HTTP/1.0 " << m_RespCode << " " << m_RespMsg << "\r\n";

    m_RespSize = GetFileSize(file);

    std::stringstream ss;
    ss << m_RespSize;
	m_RespHeaders.emplace("Content-Size", ss.str());

    for (const auto& header : m_RespHeaders)
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

void HTTPWorker::LogRequest()
{
    std::string ipAddr = m_Sock.remote_endpoint().address().to_string();
    string verb = m_Method;
    std::transform(verb.begin(), verb.end(), verb.begin(), ::toupper);

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%d/%b/%Y:%H:%M:%S %z");

    string referer = "-";
    const auto& refererIt = m_Headers.find("Referer");
    if (refererIt != m_Headers.cend())
        referer = refererIt->second;

    string userAgent = "-";
    const auto& userAgentIt = m_Headers.find("User-Agent");
    if (userAgentIt != m_Headers.cend())
        userAgent = userAgentIt->second;

    printf("%s - - [%s] \"%s %s HTTP/1.0\" %d %d \"%s\" \"%s\"\n",
        ipAddr.c_str(), ss.str().c_str(),
        verb.c_str(), m_Path.c_str(),
        m_RespCode, m_RespSize,
        referer.c_str(), userAgent.c_str());
}

string HTTPWorker::GetDirectoryListHTML(const string& path, const string& realPath)
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

void HTTPWorker::MethodGetHandler(const bool& sendBody /*= true*/)
{
    std::ifstream file;
    bool directoryListing = false;

    string realPath = mp_Server->GetConfig()->GetContentRoot() + m_Path;

    if (m_Path.back() == '/')
    {
        bool indexFound = false;
        const vector<string>& indexes = mp_Server->GetConfig()->GetIndexes();
        for (const auto& indexFile : indexes)
        {
            file.open(realPath + indexFile, std::ios::in | std::ios::binary);

            if (file)
            {
                realPath += indexFile;
                indexFound = true;
                break;
            }
        }

        if (!indexFound)
        {
            directoryListing = true;
        }
    }
    else
    {
        file.open(realPath, std::ios::in | std::ios::binary);
    }

    if (directoryListing)
    {
        m_RespCode = 200;
        m_RespMsg = "OK";
        const string& html = GetDirectoryListHTML(m_Path, realPath);
        WriteBufferResponse(html);
    }
    else if (file)
    {
        m_RespCode = 200;
        m_RespMsg = "OK";

        vector<string> filenameParts = StringSplit(Basename(realPath), ".");
        const string& ext = filenameParts.back();

        const string& mimeType = mp_Server->GetConfig()->GetMimeType(ext);
        m_RespHeaders.emplace("Content-Type", mimeType);

        WriteFileResponse(file);
    }
    else
    {
        m_RespCode = 404;
        m_RespMsg = "Not Found";

        file.close();
        file.open(mp_Server->GetConfig()->Get404File());
        m_RespHeaders.emplace("Content-Type", "text/html");

        WriteFileResponse(file);
    }
}

void HTTPWorker::MethodPostHandler()
{

}
