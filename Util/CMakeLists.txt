
SET(_target Util)

FILE(
    GLOB_RECURSE
    _public_list
    "Private/*.h"
    "Private/*.c"
)

FILE(
    GLOB_RECURSE
    _private_list
    "Private/*.h"
    "Private/*.c"
)

ADD_LIBRARY(
    ${_target}
    ${_public_list}
    ${_private_list}
)

TARGET_INCLUDE_DIRECTORIES(
    ${_target}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

SET_TARGET_PROPERTIES(
    ${_target}
    PROPERTIES
        C_STANDARD 17
)

TARGET_COMPILE_DEFINITIONS(
    ${_target}
    PUBLIC
        # Disable VS "not secure" warnings
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

TARGET_LINK_LIBRARIES(
    ${_target}
    PUBLIC
        Threads::Threads
)