cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

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
add_compile_definitions(RAPI_EXPORT)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT ${PROJECT_NAME}_Runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${PROJECT_NAME}_Runtime
                NAMELINK_COMPONENT ${PROJECT_NAME}_Development
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${PROJECT_NAME}_Development
)
install(DIRECTORY ${HEADER_DIR}/rune DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${PROJECT_NAME}Version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)
