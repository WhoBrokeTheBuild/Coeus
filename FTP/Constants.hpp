#ifndef COEUS_FTP_CONSTANTS_HPP
#define COEUS_FTP_CONSTANTS_HPP

enum class TransferMode
{
    ASCII,
    EBCDIC,
    BINARY,
    LOCAL
};

inline const char **TransferModeNames()
{
    static const char *names[] = { "ASCII", "EBCDIC", "Binary", "Local", nullptr };
    return names;
}

inline const char *TransferModeName(TransferMode e) { return TransferModeNames()[static_cast<int>(e)]; }

#endif // COEUS_FTP_CONSTANTS_HPP
