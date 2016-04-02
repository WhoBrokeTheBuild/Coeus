#include "POPWorker.hpp"

POPWorker::POPWorker(POPServer* pServer, tcp::socket sock) :
    mp_Server(pServer),
    m_Sock(std::move(sock))
{ }

void POPWorker::Run()
{

}
