#include "SMTPSession.hpp"

string SMTPSession::Init()
{
    return "220 " + mp_Server->GetConfig()->GetServerName();
}

string SMTPSession::RunCommand(const string& command)
{
    return "500 Unrecognized Command";
}
