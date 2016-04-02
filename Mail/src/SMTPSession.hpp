#ifndef COEUS_SMTP_SESSION_HPP
#define COEUS_SMTP_SESSION_HPP

#include "ISession.hpp"
#include "SMTPServer.hpp"

class SMTPSession :
    public ISession
{
public:

    SMTPSession(SMTPServer* pServer) :
        mp_Server(pServer)
    { }

    string Init();
    string RunCommand(const string& command);

public:

    SMTPServer* mp_Server;

};

#endif // COEUS_SMTP_SESSION_HPP
