set(SUBMODULE_EXECUTABLE rune-profiler)

list(APPEND SUBMODULE_FILES
        src/profiler.c
)

set(SUBMODULE_HEADER_DIR ${CMAKE_SOURCE_DIR}/profiler/include)

include(${CMAKE_SOURCE_DIR}/CMake/SubmoduleDefines.cmake)
