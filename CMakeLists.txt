cmake_minimum_required(VERSION 3.20)
project(rune-engine VERSION 0.64.0 DESCRIPTION "High performance game engine designed for Quake-style shooters")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(cglm REQUIRED)
find_package(json-c REQUIRED)
find_package(Threads REQUIRED)

set(ENGINE_HEADER_DIR ${CMAKE_SOURCE_DIR}/include)
list(APPEND SUBMODULE_INCLUDE_DIRS
        ${ENGINE_HEADER_DIR}
        ${Vulkan_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIR}
        ${OPENAL_INCLUDE_DIR}
)

add_subdirectory("engine")
add_subdirectory("editor")
add_subdirectory("profiler")
add_subdirectory("doc")

install(DIRECTORY ${ENGINE_HEADER_DIR}/rune DESTINATION include)
