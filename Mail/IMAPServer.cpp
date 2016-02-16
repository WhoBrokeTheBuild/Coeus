#include "IMAPServer.hpp"

IMAPServer::IMAPServer(const Config& config) :
    m_Config(config),
    m_IoSvc()
{ }

void IMAPServer::Run()
{
    printf("IMAP Server running on port %d\n", m_Config.GetIMAPPort());

}
