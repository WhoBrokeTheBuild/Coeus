#include "SMTPWorker.hpp"

SMTPWorker::SMTPWorker(SMTPServer* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock))
{ }

void SMTPWorker::Run()
{
    
}
