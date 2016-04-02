#ifndef COEUS_MAIL_CONFIG_HPP
#define COEUS_MAIL_CONFIG_HPP

#include <string>

using std::string;

class Config
{
public:

    Config();
    Config(const Config&) = default;
    Config& operator=(const Config&) = default;
    ~Config() = default;

    bool Load(const string& filename);

    inline string GetServerName() const { return m_ServerName; }
    inline unsigned int GetSMTPPort() const { return m_SMTPPort; }
    inline unsigned int GetPOPPort() const { return m_POPPort; }
    inline unsigned int GetIMAPPort() const { return m_IMAPPort; }

private:

    string m_ServerName;
    unsigned int m_SMTPPort = 0;
    unsigned int m_POPPort  = 0;
    unsigned int m_IMAPPort = 0;

};

#endif // COEUS_MAIL_CONFIG_HPP
