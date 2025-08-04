set(SUBMODULE_EXECUTABLE rune-editor)

list(APPEND SUBMODULE_FILES
        src/editor.c
)

list(APPEND SUBMODULE_LINK_LIBS
        rune-engine
)

set(SUBMODULE_HEADER_DIR ${CMAKE_SOURCE_DIR}/editor/include)

include(${CMAKE_SOURCE_DIR}/CMake/SubmoduleDefines.cmake)
