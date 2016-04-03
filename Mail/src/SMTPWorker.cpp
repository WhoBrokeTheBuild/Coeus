#include "SMTPWorker.hpp"

#include "SMTPServer.hpp"
#include <Functions.hpp>

SMTPWorker::SMTPWorker(SMTPServer* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock))
{
    m_CommandHandlers.emplace("helo", [this](string data) {
        SendMessage("250 OK");
    });
    m_CommandHandlers.emplace("quit", [this](string data) {
        SendMessage("221 Goodbye");
    });
    m_CommandHandlers.emplace("mail", [this](string data) { CommandMail(data); });
    m_CommandHandlers.emplace("rcpt", [this](string data) { CommandRecipient(data); });
    m_CommandHandlers.emplace("data", [this](string data) { CommandData(data); });
}

void SMTPWorker::Run()
{
    m_Running = true;

    SendMessage("220 " + mp_Server->GetConfig()->GetServerName());
    while (m_Running)
    {
        try
        {
            asio::streambuf buf;
            size_t bytesRead = asio::read_until(m_Sock, buf, "\n");
            std::stringstream ss({asio::buffers_begin(buf.data()), asio::buffers_end(buf.data())});

            string line;
            getline(ss, line);

            printf("%s\n", line.c_str());

            ss.str(line);
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
                SendMessage("500 Unrecognized Command");
            }
        }
        catch (...)
        {
            Stop();
        }
    }
}

void SMTPWorker::Stop()
{
    m_Sock.close();
    m_Running = false;
}

void SMTPWorker::CommandMail(string data)
{

}

void SMTPWorker::CommandRecipient(string data)
{

}

void SMTPWorker::CommandData(string data)
{

}

void SMTPWorker::SendMessage(string msg)
{
    msg += "\r\n";
    size_t bytesWritten = asio::write(m_Sock, asio::buffer(msg, msg.size()));

    if (bytesWritten == 0)
    {
        Stop();
    }
}
