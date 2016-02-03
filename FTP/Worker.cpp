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

}

Worker::~Worker()
{

}

void Worker::Run()
{
    bool ret = SendMessage("220 Coeus FTP Server Ready\r\n");

    if (!ret)
    {
        Stop();
        return;
    }

    for (;;)
    {
        asio::streambuf buf;
    	size_t bytesRead = asio::read_until(m_Sock, buf, "\r\n");
        string line = {asio::buffers_begin(buf.data()), asio::buffers_end(buf.data())};

        printf("%s\n", line.c_str());

        std::stringstream ss(line);
        string cmd;
        ss >> cmd;

        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (cmd == "user")
        {
            ret = SendMessage("230 Access Granted");
        }
        else if (cmd == "pwd")
        {
            ret = SendMessage("257 \"" + m_Pwd + "\" is current directory.");
        }
        else if (cmd == "type")
        {
            string type;
            ss >> type;

            std::transform(type.begin(), type.end(), type.begin(), ::tolower);

            bool foundType = true;
            if (type == "a")
            {
                m_TransMode = TransferMode::ASCII;
            }
            else if (type == "e")
            {
                m_TransMode = TransferMode::EBCDIC;
            }
            else if (type == "i")
            {
                m_TransMode = TransferMode::BINARY;
            }
            else if (type == "l")
            {
                m_TransMode = TransferMode::LOCAL;
            }
            else
            {
                ret = SendMessage("501 Invalid Argument");
                foundType = false;
            }

            if (foundType)
            {
                ret = SendMessage("200 Mode is now " + string(TransferModeName(m_TransMode)));
            }
        }
        else if (cmd == "port")
        {
            string data;
            ss >> data;
            std::stringstream splitSs(data);

            vector<int> parts;
            string tmpStr;
            int tmpInt;
            while (getline(splitSs, tmpStr, ','))
            {
                std::stringstream tmpSs(tmpStr);
                tmpSs >> tmpInt;
                parts.push_back(tmpInt);
            }

            if (parts.size() < 6)
            {
                ret = SendMessage("501 Invalid Argument");
            }
            else
            {
                unsigned short fullPort = (parts[4] * 256) + parts[5];

                std::stringstream tmpSs;
                tmpSs << parts[0] << "." << parts[1] << "." << parts[2] << "." << parts[3];

                m_DataEndpoint = tcp::endpoint(asio::ip::address::from_string(tmpSs.str()), fullPort);

                ret = SendMessage("200 Data Address Set");
            }
        }
        else if (cmd == "list")
        {
            asio::streambuf resp;
            std::ostream respStream(&resp);

        	DIR *dir;
        	struct dirent *ent;

            string path = mp_Server->GetConfig()->GetRootDir() + "/" + m_Pwd;
        	dir = opendir(path.c_str());
        	if (dir != NULL)
        	{
        		while ((ent = readdir(dir)) != NULL)
        		{
                    string line;
        			string name = string(ent->d_name);
                    string filename = path + "/" + name;

                    respStream << "+i8388621.48594,m825718503,";

        			if (name != "." && name != "..")
        			{

            			switch (ent->d_type)
            			{
            			case DT_REG:
                            {
                                respStream << "r,";

                                std::ifstream file(filename);
                                size_t size = GetFileSize(file);
                                file.close();

                                respStream << "s" << size << ",";
                            }
            				break;
            			case DT_DIR:

                            respStream << "/,";

            				break;
            			}

                        respStream << "\t" << name << "\r\n";
        			}

        		}

        		closedir(dir);
        	}

            SendMessage("150 Opening Data Connection to Send File List");
            m_DataSock.connect(m_DataEndpoint);
            asio::write(m_DataSock, resp);
            m_DataSock.close();
            ret = SendMessage("226 Transfer complete");
        }
        else if (cmd == "cwd")
        {
            ss >> m_Pwd;

            ret = SendMessage("250 Changed directory to \"" + m_Pwd + "\"");
        }
        else if (cmd == "mkd")
        {
            string dir;
            ss >> dir;

            dir = mp_Server->GetConfig()->GetRootDir() + "/" + dir;

            if (MakeDir(dir))
            {
                ret = SendMessage("257 \"" + dir + "\" Created");
            }

        }
        else
        {
            ret = SendMessage("502 Command Not Implemented");
        }

        if (!ret)
        {
            Stop();
            return;
        }
    }
}

void Worker::Stop()
{
    m_Sock.close();
}

bool Worker::SendMessage(string msg)
{
    msg += "\r\n";

    size_t bytesWritten = asio::write(m_Sock, asio::buffer(msg, msg.size()));
    if (bytesWritten == 0)
        return false;

    return true;
}
