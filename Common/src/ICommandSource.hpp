#ifndef COEUS_ICOMMAND_SOURCE_HPP
#define COEUS_ICOMMAND_SOURCE_HPP

#include <string>

using std::string;

class ICommandPipe
{
public:

    string GetNextCommand() = 0;
    void SendCommand(const string& str) = 0;

};

#endif // COEUS_ICOMMAND_SOURCE_HPP
