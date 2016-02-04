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

    inline unsigned int GetSMTPPort() const { return m_SMTPPort; }
    inline unsigned int GetPOPPort() const { return m_POPPort; }
    inline unsigned int GetIMAPPort() const { return m_IMAPPort; }

private:

    unsigned int m_SMTPPort;
    unsigned int m_POPPort;
    unsigned int m_IMAPPort;

};

#endif // COEUS_MAIL_CONFIG_HPP
