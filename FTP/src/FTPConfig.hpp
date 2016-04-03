#ifndef COEUS_FTP_CONFIG_HPP
#define COEUS_FTP_CONFIG_HPP

#include <string>

using std::string;

class FTPConfig
{
public:

    FTPConfig();
    FTPConfig(const FTPConfig&) = default;
    FTPConfig& operator=(const FTPConfig&) = default;
    ~FTPConfig() = default;

    bool Load(const string& filename);

    inline unsigned int GetPort() const { return m_Port; }
    inline void SetPort(const unsigned int& port) { m_Port = port; }

    inline string GetRootDir() const { return m_RootDir; }
    inline void SetRootDir(const string& rootDir) { m_RootDir = rootDir; }

private:

    unsigned int
        m_Port;

    string
        m_RootDir;

};

#endif // COEUS_FTP_CONFIG_HPP
