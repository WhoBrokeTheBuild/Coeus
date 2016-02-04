#ifndef COEUS_HTTP_CONFIG_HPP
#define COEUS_HTTP_CONFIG_HPP

#include <string>
#include <map>
#include <vector>

using std::string;
using std::map;
using std::vector;

class Config
{
public:

    Config();
    Config(const Config&) = default;
    Config& operator=(const Config&) = default;
    ~Config() = default;

    bool Load(const string& filename);
    bool LoadMime(const string& filename);

    inline unsigned int GetPort() const { return m_Port; }
    inline string GetContentRoot() const { return m_ContentRoot; }
    inline string Get404File() const { return m_404File; }

	vector<string> GetIndexes( void ) const { return m_Indexes; }

    string GetMimeType(const string& extension) const;

private:

    unsigned int m_Port;

    string m_ContentRoot;
    string m_404File;

	vector<string> m_Indexes;

    map<string, vector<string>>	m_MimeTypes;

};

#endif // COEUS_HTTP_CONFIG_HPP
