list(APPEND SUBMODULE_INCLUDE_DIRS ${SUBMODULE_HEADER_DIR})

include(EnableCFLAG)
if (MSVC)
        enable_c_compiler_flag_if_supported("/GS")
else ()
        enable_c_compiler_flag_if_supported("-fstack-protector-all")
endif ()

if (WIN32)
        find_package(dlfcn-win32 REQUIRED)
        find_package(DBGHELP REQUIRED)
        list(APPEND SUBMODULE_LINK_LIBS dlfcn-win32::dl ${DBGHELP_LIBRARY})
endif()

if (DEFINED SUBMODULE_LIBRARY)
        add_definitions(-DRAPI_EXPORT)
        set(SUBMODULE_BINARY ${SUBMODULE_LIBRARY})
        add_library(${SUBMODULE_BINARY} SHARED ${SUBMODULE_FILES})
        set_target_properties(${SUBMODULE_BINARY} PROPERTIES VERSION ${PROJECT_VERSION})
        set_target_properties(${SUBMODULE_BINARY} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
        target_include_directories(${SUBMODULE_BINARY} PUBLIC ${SUBMODULE_INCLUDE_DIRS})
        target_link_libraries(${SUBMODULE_BINARY} PUBLIC ${SUBMODULE_LINK_LIBS})
elseif (DEFINED SUBMODULE_EXECUTABLE)
        set(SUBMODULE_BINARY ${SUBMODULE_EXECUTABLE})
        add_executable(${SUBMODULE_BINARY} ${SUBMODULE_FILES})
        set_target_properties(${SUBMODULE_BINARY} PROPERTIES VERSION ${PROJECT_VERSION})
        target_include_directories(${SUBMODULE_BINARY} PRIVATE ${SUBMODULE_INCLUDE_DIRS})
        target_link_libraries(${SUBMODULE_BINARY} PRIVATE ${SUBMODULE_LINK_LIBS})
else ()
        message(FATAL_ERROR "Unknown submodule type defined by submodule ${CMAKE_CURRENT_SOURCE_DIR}")
endif ()

target_compile_definitions(${SUBMODULE_BINARY} PUBLIC RUNE_VER="${CMAKE_PROJECT_VERSION}")
target_compile_definitions(${SUBMODULE_BINARY} PUBLIC RUNE_VER_MAJOR="${CMAKE_PROJECT_VERSION_MAJOR}")
target_compile_definitions(${SUBMODULE_BINARY} PUBLIC RUNE_VER_MINOR="${CMAKE_PROJECT_VERSION_MINOR}")
target_compile_definitions(${SUBMODULE_BINARY} PUBLIC RUNE_VER_PATCH="${CMAKE_PROJECT_VERSION_PATCH}")

option(ENABLE_PROFILING "Enable profiling")
if (ENABLE_PROFILING)
        target_compile_definitions(${SUBMODULE_BINARY} PUBLIC RUNE_PROFILING)
endif ()

include(GNUInstallDirs)
install(TARGETS ${SUBMODULE_BINARY}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                COMPONENT ${SUBMODULE_BINARY}_Runtime
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${SUBMODULE_BINARY}_Runtime
                NAMELINK_COMPONENT ${SUBMODULE_BINARY}_Development
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                COMPONENT ${SUBMODULE_BINARY}_Development
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${SUBMODULE_BINARY}Version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)
