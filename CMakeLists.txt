cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

project(rune-engine VERSION 0.61.0 DESCRIPTION "High performance game engine designed for Quake-style shooters")

list(APPEND SOURCE_FILES
        core/abort.c
        core/alloc.c
        core/callbacks.c
        core/init.c
        core/logging.c
        core/network.c
)

list(APPEND SOURCE_FILES
        ui/window.c
        ui/input.c
        ui/renderer.c
)

set(HEADER_DIR include)

add_compile_definitions(VERSION="${PROJECT_VERSION}")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY ${HEADER_DIR}/rune DESTINATION include)
