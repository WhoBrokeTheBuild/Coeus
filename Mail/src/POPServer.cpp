#include "POPServer.hpp"

POPServer::POPServer(const Config& config) :
    m_Config(config),
    m_IoSvc()
{ }

void POPServer::Run()
{
    printf("POP Server running on port %d\n", m_Config.GetPOPPort());

}
