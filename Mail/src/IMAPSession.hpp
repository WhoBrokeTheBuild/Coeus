#ifndef COEUS_IMAP_SESSION_HPP
#define COEUS_IMAP_SESSION_HPP

class IMAPSession :
    public ISession
{
public:

    string RunCommand(const string& command);

private:

};

#endif // COEUS_IMAP_SESSION_HPP
