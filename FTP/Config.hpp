#ifndef COEUS_HTTP_CONFIG_HPP
#define COEUS_HTTP_CONFIG_HPP

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

    inline unsigned int GetPort() const { return m_Port; }
    inline string GetRootDir() const { return m_RootDir; }

private:

    unsigned int m_Port;

    string m_RootDir;

};

#endif // COEUS_HTTP_CONFIG_HPP
