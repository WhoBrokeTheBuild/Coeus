
SET(_target CFTP)

FILE(
    GLOB_RECURSE
    _private_list
    "Private/*.h"
    "Private/*.c"
)

ADD_EXECUTABLE(
    ${_target}
    ${_private_list}
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
    PRIVATE
        Util
)