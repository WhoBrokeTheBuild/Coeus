CMAKE_MINIMUM_REQUIRED(VERSION 3.20 FATAL_ERROR)

# Allow for custom CMake modules
LIST(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake)

# Allow PROJECT() to manage versions
IF(POLICY CMP0048)
    CMAKE_POLICY(SET CMP0048 NEW)
ENDIF()

# Process DEPFILEs sent to Ninja
IF(POLICY CMP0116)
    CMAKE_POLICY(SET CMP0116 NEW)
ENDIF()

PROJECT(Coeus)

INCLUDE(ProjectVersion)

FIND_PACKAGE(Threads REQUIRED)

ADD_SUBDIRECTORY(Util)

ADD_SUBDIRECTORY(FTP)
ADD_SUBDIRECTORY(HTTP)
ADD_SUBDIRECTORY(IMAP)
ADD_SUBDIRECTORY(POP)
ADD_SUBDIRECTORY(SMTP)