#include "Worker.hpp"

#include "Server.hpp"
#include "Functions.hpp"
#include <vector>
#include <dirent.h>

using std::vector;

Worker::Worker(Server* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock)),
    m_DataSock(pServer->GetIoService())
{
    m_CommandHandlers.emplace("user", [this](string data) {
        SendMessage("230 Access Granted");
    });

    m_CommandHandlers.emplace("pwd", [this](string data) {
        SendMessage("257 \"" + m_Pwd + "\" is current directory.");
    });

    m_CommandHandlers.emplace("mkd", [this](string data) {
        data = mp_Server->GetConfig()->GetRootDir() + "/" + data;

        if (MakeDir(data))
        {
            SendMessage("257 \"" + data + "\" Created");
        }
    });

    m_CommandHandlers.emplace("type", [this](string data) { CommandType(data); });
    m_CommandHandlers.emplace("port", [this](string data) { CommandPort(data); });
    m_CommandHandlers.emplace("list", [this](string data) { CommandList(data); });
    m_CommandHandlers.emplace("cwd", [this](string data) { CommandChangeDir(data); });
    m_CommandHandlers.emplace("cdup", [this](string data) { CommandChangeDir(".."); });
    m_CommandHandlers.emplace("retr", [this](string data) { CommandReturn(data); });
}

Worker::~Worker()
{

}

void Worker::Run()
{
    m_Running = true;

    SendMessage("220 Coeus FTP Server Ready\r\n");
    while (m_Running)
    {
        asio::streambuf buf;
    	size_t bytesRead = asio::read_until(m_Sock, buf, "\r\n");
        string line = {asio::buffers_begin(buf.data()), asio::buffers_end(buf.data())};

        printf("%s\n", line.c_str());

        std::stringstream ss(line);
        string cmd;
        ss >> cmd;

        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        string data;
        getline(ss, data);
        data = StringTrim(data);

        const auto& handlerIt = m_CommandHandlers.find(cmd);
        if (handlerIt != m_CommandHandlers.cend())
        {
            handlerIt->second(data);
        }
        else
        {
            SendMessage("502 Command Not Implemented");
        }
    }
}

void Worker::Stop()
{
    m_Sock.close();
    m_Running = false;
}

void Worker::CommandType(string data)
{
    if (data.empty())
    {
        SendMessage("501 Invalid Argument");
        return;
    }

    char type = tolower(data.front());

    bool foundType = true;
    switch (type)
    {
    case 'a':
        m_TransMode = TransferMode::ASCII;
        break;
    case 'e':
        m_TransMode = TransferMode::EBCDIC;
        break;
    case 'i':
        m_TransMode = TransferMode::BINARY;
        break;
    case 'l':
        m_TransMode = TransferMode::LOCAL;
        break;
    default:
        foundType = false;
    }

    if (!foundType)
    {
        SendMessage("501 Invalid Argument");
        return;
    }

    SendMessage("200 Mode is now " + string(TransferModeName(m_TransMode)));
}

void Worker::CommandPort(string data)
{
    unsigned short addr[] = { 0, 0, 0, 0 };
    unsigned short port[] = { 0, 0 };

    int count = sscanf(data.c_str(),
        "%hu,%hu,%hu,%hu,%hu,%hu",
        &addr[0], &addr[1], &addr[2], &addr[3],
        &port[0], &port[1]);

    if (count < 6)
    {
        SendMessage("501 Invalid Argument");
    }
    else
    {
        unsigned short fullPort = (port[0] * 256) + port[1];

        std::stringstream tmpSs;
        tmpSs << addr[0] << "." << addr[1] << "." << addr[2] << "." << addr[3];

        m_DataEndpoint = tcp::endpoint(asio::ip::address::from_string(tmpSs.str()), fullPort);

        SendMessage("200 Data Address Set");
    }
}

void Worker::CommandList(string data)
{
    asio::streambuf resp;
    std::ostream respStream(&resp);

    string path = mp_Server->GetConfig()->GetRootDir() + "/" + m_Pwd;

    const vector<DirectoryEntry>& entries = GetDirectoryList(path);

    for (const auto& ent : entries)
    {
        if (ent.Filename == "." || ent.Filename == "..")
            continue;

        string line;
        string filename = path + "/" + ent.Filename;

        respStream << "+i" << ent.Id << ",m825718503,";

        if (ent.Filename != "." && ent.Filename != "..")
        {
            switch (ent.Type)
            {
            case DirectoryEntry::EntryType::File:
                {
                    respStream << "r,";

                    std::ifstream file(filename);
                    size_t size = GetFileSize(file);
                    file.close();

                    respStream << "s" << size << ",";
                }
                break;
            case DirectoryEntry::EntryType::Directory:

                respStream << "/,";

                break;
            }

            respStream << "\t" << ent.Filename << "\r\n";
        }

    }

    SendMessage("150 Opening Data Connection to Send File List");

    m_DataSock.connect(m_DataEndpoint);
    asio::write(m_DataSock, resp);
    m_DataSock.close();

    SendMessage("226 Transfer complete");
}

void Worker::CommandChangeDir(string data)
{
    if (data == "..")
    {
        if (m_Pwd == "/")
        {
            SendMessage("550 Cannot change directory");
            return;
        }

        const vector<string>& dirParts = StringSplit(m_Pwd, "/");
        m_Pwd = "/";
        for (const auto& str : dirParts)
        {
            m_Pwd += str + "/";
        }
    }
    else
    {
        m_Pwd = data;
    }

    SendMessage("250 Changed directory to \"" + m_Pwd + "\"");
}

void Worker::CommandReturn(string data)
{
    if (data.empty())
    {
        SendMessage("501 Invalid Argument");
    }

    string filename = mp_Server->GetConfig()->GetRootDir();
    if (data.front() == '/')
    {
        filename += data;
    }
    else
    {
        filename += m_Pwd + "/" + data;
    }

    std::ifstream file(filename);

    if (!file)
    {

    }

    SendMessage("150 Opening Data Connection to Send File");

    m_DataSock.connect(m_DataEndpoint);

    const int TMP_BUFFER_SIZE = 4096;

    char tmp_buffer[TMP_BUFFER_SIZE];
    std::streamsize n;
    do
    {
        file.read(tmp_buffer, TMP_BUFFER_SIZE);
        n = file.gcount();

        if (n == 0)
            break;

        asio::write(m_DataSock, asio::buffer(tmp_buffer, n));

        if (!file)
            break;
    } while (n > 0);

    m_DataSock.close();

    SendMessage("226 Transfer complete");
}

void Worker::SendMessage(string msg)
{
    msg += "\r\n";

    size_t bytesWritten = asio::write(m_Sock, asio::buffer(msg, msg.size()));

    if (bytesWritten == 0)
    {
        Stop();
    }
}
