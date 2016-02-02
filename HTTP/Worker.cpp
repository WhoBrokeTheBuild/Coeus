#include "Worker.hpp"

#include "Server.hpp"
#include "Functions.hpp"
#include <dirent.h>

Worker::Worker(Server* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock))
{

}

Worker::~Worker()
{

}

void Worker::Run()
{
    GetHeader();
    HandleRequest();
}

void Worker::GetHeader()
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

        m_Headers.emplace(headerSplit[0], headerSplit[1]);
    }
}

void Worker::HandleRequest()
{
    if (m_Method == "get")
    {
    	std::ifstream file;
    	bool directoryListing = false;

    	string realPath = mp_Server->GetConfig()->GetContentRoot() + m_Path;

    	if (m_Path.back() == '/')
    	{
    		bool defaultFound = false;
    		const vector<string>& indexes = mp_Server->GetConfig()->GetIndexes();
    		for (const auto& indexFile : indexes)
    		{
    			file.open(realPath + indexFile, std::ios::in | std::ios::binary);

    			if (file)
    			{
    				realPath += indexFile;
    				defaultFound = true;
    				break;
    			}
    		}

    		if (!defaultFound)
    		{
    			directoryListing = true;
    		}
    	}
    	else
    		file.open(realPath, std::ios::in | std::ios::binary);

    	if (!directoryListing && !file)
    	{
    		const string& response = "HTTP/1.0 404 Not Found";

            asio::streambuf resp;
            std::ostream respStream(&resp);
            respStream << "HTTP/1.0 404 Not Found\r\n";
            asio::write(m_Sock, resp);

    		file.close();
    		file.open(mp_Server->GetConfig()->Get404File());
    	}
    	else
    	{
    		const string& response = "HTTP/1.0 200 OK";

            asio::streambuf resp;
            std::ostream respStream(&resp);
            respStream << "HTTP/1.0 200 OK\r\n";
            asio::write(m_Sock, resp);
    	}

    	if (directoryListing)
    	{
            SendDirectoryList(m_Path, realPath);
    	}
    	else
    	{
            asio::streambuf resp;
            std::ostream respStream(&resp);
            respStream << "HTTP/1.0 200 OK\r\n";

            vector<string> filenameParts = StringSplit(Basename(realPath), ".");
    		const string& ext = filenameParts.back();

    		const string& mimeType = mp_Server->GetConfig()->GetMimeType(ext);
            respStream << "Content-Type: " << mimeType << "\r\n";

    		const std::streamsize& fileSize = GetFileSize(file);
    		respStream << "Content-Size: " << fileSize << "\r\n";

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
    	}
    }
    else if (m_Method == "post")
    {

    }
}


void Worker::SendDirectoryList(const string& path, const string& realPath)
{
	std::stringstream output;

	output << "<html>\r\n"
		<< "<head>\r\n"
		<< "<title>Directory Listing for " << path << "</title>\r\n"
		<< "</head>\r\n"
		<< "<body>\r\n"
		<< "<h1>Directory Listing for " << path << "</h1>\r\n"
		<< "<table>\r\n"
		<< "<tr><th>Icon</th><th>Name</th><th>Type</th></tr>";

	DIR *dir;
	struct dirent *ent;

	dir = opendir(realPath.c_str());
	if (dir != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			string type = "";
			string name = string(ent->d_name);
			string href = "";

			switch (ent->d_type)
			{
			case DT_REG:

				type = "File";

				break;
			case DT_DIR:

				type = "Directory";

				break;
			}

			if (name != "." && name != "..")
			{
				href = path + name;
			}

			if (name == ".." && path.length() > 1)
			{
				string tmpPath = path;
				tmpPath.pop_back();
				size_t lastSlash = tmpPath.rfind('/');
				if (lastSlash != string::npos)
				{
					href = tmpPath.substr(0, lastSlash + 1);
				}
			}

			output << "<tr><td>" << type.front() << "</td><td>";

			if (href.length() > 0)
			{
				output << "<a href=\"" << href << "\">" << name << "</a>";
			}
			else
				output << name;

			output << "</td><td>" << type << "</td></tr>\r\n";
		}

		closedir(dir);

	}

	output << "</table>\r\n"
		<< "</body>\r\n"
		<< "</html>\r\n";

	string outStr = output.str();

    asio::streambuf resp;
    std::ostream respStream(&resp);
    respStream << "HTTP/1.0 200 OK\r\n";
	respStream << "Content-Size: " << outStr.length() << "\r\n";
	respStream << "\r\n";
    respStream << outStr;
    
    asio::write(m_Sock, resp);
}
