set(SUBMODULE_LIBRARY rune-engine)

list(APPEND SUBMODULE_FILES
        core/abort.c
        core/alloc.c
        core/callbacks.c
        core/config.c
        core/console.c
        core/init.c
        core/logging.c
        core/mesh.c
        core/mod.c
        core/object.c
        core/profiling.c
        core/thread.c
)

list(APPEND SUBMODULE_FILES
        render/directx/renderer.c
        render/vulkan/context.c
        render/vulkan/device.c
        render/vulkan/fence.c
        render/vulkan/framebuffer.c
        render/vulkan/image.c
        render/vulkan/renderer.c
        render/vulkan/renderpass.c
        render/vulkan/swapchain.c
        ui/input.c
        ui/panel.c
        ui/window.c
        sound/sound.c
)

list(APPEND SUBMODULE_LINK_LIBS
        ${Vulkan_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${OPENAL_LIBRARY}
        json-c::json-c
        ${CMAKE_THREAD_LIBS_INIT}
)

if (WIN32)
        list(APPEND SUBMODULE_LINK_LIBS glfw3dll d3d12.lib dxgi.lib dxguid.lib)
else ()
        list(APPEND SUBMODULE_LINK_LIBS glfw)
endif ()

include(${CMAKE_SOURCE_DIR}/CMake/SubmoduleDefines.cmake)
