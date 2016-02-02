#ifndef COEUS_HTTP_CONFIG_HPP
#define COEUS_HTTP_CONFIG_HPP

#include <string>

using std::string;

class Config
{
public:

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    Config();
    ~Config() = default;

    bool Load(const string& filename);

    inline unsigned int GetCommandPort() const { return m_CommandPort; }
    inline unsigned int GetDataPort() const { return m_DataPort; }

private:

    unsigned int m_CommandPort;
    unsigned int m_DataPort;

};

#endif // COEUS_HTTP_CONFIG_HPP
