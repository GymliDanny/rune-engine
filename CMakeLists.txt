cmake_minimum_required(VERSION 3.20)
project(rune-engine VERSION 0.61.0 DESCRIPTION "High performance game engine designed for Quake-style shooters")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

list(APPEND SOURCE_FILES
        core/abort.c
        core/alloc.c
        core/callbacks.c
        core/init.c
        core/logging.c
        core/mod.c
        core/network.c
        core/thread.c
)

list(APPEND SOURCE_FILES
        render/directx/renderer.c
        render/vulkan/context.c
        render/vulkan/device.c
        render/vulkan/fence.c
        render/vulkan/framebuffer.c
        render/vulkan/image.c
        render/vulkan/renderer.c
        render/vulkan/renderpass.c
        render/vulkan/swapchain.c
        ui/window.c
        ui/input.c
)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(cglm REQUIRED)
find_package(Threads REQUIRED)

set(HEADER_DIR include)

list(APPEND INCLUDE_DIRS
        ${HEADER_DIR}
        ${Vulkan_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIR}
        ${OPENAL_INCLUDE_DIR}
)

list(APPEND LINK_LIBS
        ${Vulkan_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${OPENAL_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
)

if (WIN32)
        find_package(dlfcn-win32 REQUIRED)
        set(GLFW_LIBRARIES glfw3dll)
        set(DX12_LIBRARIES d3d12.lib dxgi.lib dxguid.lib)
        set(DL_LIBRARIES dlfcn-win32::dl)
        list(APPEND LINK_LIBS ${DX12_LIBRARIES} ${DL_LIBRARIES})
else()
        set(GLFW_LIBRARIES glfw)
endif()

list(APPEND LINK_LIBS ${GLFW_LIBRARIES})

add_compile_definitions(VERSION="${PROJECT_VERSION}")
add_compile_definitions(RAPI_EXPORT)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

add_subdirectory("doc")

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
