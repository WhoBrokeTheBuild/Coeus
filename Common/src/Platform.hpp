#ifndef COEUS_PLATFORM_HPP
#define COEUS_PLATFORM_HPP

#if defined(_WIN32)
#   define COEUS_OS_WINDOWS
#elif defined(__linux)
#   define COEUS_OS_LINUX
#endif

#endif // COEUS_PLATFORM_HPP