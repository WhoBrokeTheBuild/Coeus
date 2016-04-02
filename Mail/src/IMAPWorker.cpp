#include "IMAPWorker.hpp"

IMAPWorker::IMAPWorker(IMAPServer* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock))
{ }

void IMAPWorker::Run()
{

}
