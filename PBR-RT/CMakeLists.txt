# CMakeLists.txt : Anito Engine - DOST-PCIEERD Project Anito
# DLSU GAME Lab - AAA Game Engine Development
# Principal Investigator: Neil Patrick Del Gallego, Ph.D.
#
cmake_minimum_required(VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("AnitoEngine" VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(ANITO_BUILD_SAMPLES "Build Anito Engine samples" ON)
option(ANITO_USE_VULKAN "Use Vulkan backend for bgfx" ON)

# =============================================================================
# Dependencies Setup
# =============================================================================

# Find or fetch bgfx
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)  # Don't build tools to avoid linking issues
set(BGFX_BUILD_TOOLS_SHADER OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Set bgfx paths for bgfx.cmake
set(BGFX_DIR "${CMAKE_SOURCE_DIR}/external/bgfx" CACHE STRING "bgfx directory")
set(BX_DIR "${CMAKE_SOURCE_DIR}/external/bgfx/bx" CACHE STRING "bx directory")
set(BIMG_DIR "${CMAKE_SOURCE_DIR}/external/bgfx/bimg" CACHE STRING "bimg directory")

# Use bgfx.cmake wrapper
set(BGFX_CMAKE_DIR "${CMAKE_SOURCE_DIR}/external/bgfx.cmake")
if(EXISTS "${BGFX_CMAKE_DIR}/CMakeLists.txt")
    # Use shorter binary directory path to avoid MAX_PATH issues on Windows
    add_subdirectory(${BGFX_CMAKE_DIR} bgfx EXCLUDE_FROM_ALL)
else()
    message(FATAL_ERROR "bgfx.cmake not found at ${BGFX_CMAKE_DIR}. Please clone bgfx.cmake")
endif()

# GLM (header-only)
set(GLM_DIR "${CMAKE_SOURCE_DIR}/external/glm")
if(NOT EXISTS "${GLM_DIR}/glm")
    message(WARNING "GLM not found at ${GLM_DIR}. Please install GLM")
endif()

# GLFW
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")
if(EXISTS "${GLFW_DIR}/CMakeLists.txt")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_USE_HYBRID_HPG OFF CACHE BOOL "" FORCE)

    # Fix GLFW runtime library issues - globally disable RTC for this subdirectory
    if(MSVC)
        # Save current flags
        set(SAVED_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        set(SAVED_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

        # Remove /RTC1 and /GZ from flags and add /Oi for intrinsics
        string(REGEX REPLACE "/RTC[^ ]*" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        string(REGEX REPLACE "/GZ" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Oi" CACHE STRING "" FORCE)

        string(REGEX REPLACE "/RTC[^ ]*" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        string(REGEX REPLACE "/GZ" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi" CACHE STRING "" FORCE)
    endif()

    add_subdirectory(${GLFW_DIR} EXCLUDE_FROM_ALL)

    # Restore original flags
    if(MSVC)
        set(CMAKE_C_FLAGS_DEBUG "${SAVED_C_FLAGS_DEBUG}" CACHE STRING "" FORCE)
        set(CMAKE_CXX_FLAGS_DEBUG "${SAVED_CXX_FLAGS_DEBUG}" CACHE STRING "" FORCE)
    endif()
else()
    message(WARNING "GLFW not found at ${GLFW_DIR}. Please clone GLFW")
endif()

# Bullet Physics
set(BULLET_DIR "${CMAKE_SOURCE_DIR}/external/bullet3")
if(EXISTS "${BULLET_DIR}/CMakeLists.txt")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
    set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
    set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
    set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
    set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${BULLET_DIR} EXCLUDE_FROM_ALL)
else()
    message(WARNING "Bullet Physics not found at ${BULLET_DIR}. Please clone bullet3")
endif()

# ImGui + ImGuizmo
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")
set(IMGUIZMO_DIR "${CMAKE_SOURCE_DIR}/external/ImGuizmo")

# =============================================================================
# ImGui Library (Minimal - backends will be added later)
# =============================================================================
if(EXISTS "${IMGUI_DIR}/imgui.h")
    add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
    )
    target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)

    # ImGuizmo
    if(EXISTS "${IMGUIZMO_DIR}/ImGuizmo.h")
        target_sources(imgui PRIVATE
            ${IMGUIZMO_DIR}/ImGuizmo.cpp
        )
        target_include_directories(imgui PUBLIC ${IMGUIZMO_DIR})
    endif()
else()
    message(WARNING "ImGui not found at ${IMGUI_DIR}")
endif()

# =============================================================================
# Anito Engine Source Files
# =============================================================================

# Core Math & Utility
set(ANITO_MATH_SOURCES
    src/Math/AnitoVector2D.cpp
    src/Math/AnitoVector2D.h
    src/Math/AnitoVector3D.cpp
    src/Math/AnitoVector3D.h
    src/Math/AnitoVector4D.h
    src/Math/AnitoMatrix4x4.cpp
    src/Math/AnitoMatrix4x4.h
)

# Rendering Core
set(ANITO_RENDERER_SOURCES
    src/Renderer/AnitoRenderer.cpp
    src/Renderer/AnitoRenderer.h
    src/Renderer/AnitoVertexBuffer.cpp
    src/Renderer/AnitoVertexBuffer.h
    src/Renderer/AnitoIndexBuffer.cpp
    src/Renderer/AnitoIndexBuffer.h
    src/Renderer/AnitoShader.cpp
    src/Renderer/AnitoShader.h
    src/Renderer/AnitoTexture.cpp
    src/Renderer/AnitoTexture.h
    src/Renderer/AnitoMaterial.cpp
    src/Renderer/AnitoMaterial.h
    src/Renderer/AnitoRenderTexture.cpp
    src/Renderer/AnitoRenderTexture.h
    src/Renderer/AnitoMeshGenerator.cpp
    src/Renderer/AnitoMeshGenerator.h
)

# Deferred Rendering Pipeline
set(ANITO_DEFERRED_SOURCES
    src/Renderer/Deferred/AnitoDeferredRenderer.cpp
    src/Renderer/Deferred/AnitoDeferredRenderer.h
    src/Renderer/Deferred/AnitoGBuffer.cpp
    src/Renderer/Deferred/AnitoGBuffer.h
)

# Lighting & Shadows
set(ANITO_LIGHTING_SOURCES
    src/Renderer/Lighting/AnitoLight.cpp
    src/Renderer/Lighting/AnitoLight.h
    src/Renderer/Lighting/AnitoShadowMap.cpp
    src/Renderer/Lighting/AnitoShadowMap.h
)

# GameObject & Component System
set(ANITO_GAMEOBJECT_SOURCES
    src/GameObjects/AnitoGameObject.cpp
    src/GameObjects/AnitoGameObject.h
    src/GameObjects/AnitoComponent.cpp
    src/GameObjects/AnitoComponent.h
    src/GameObjects/AnitoGameObjectManager.cpp
    src/GameObjects/AnitoGameObjectManager.h
)

# Components
set(ANITO_COMPONENT_SOURCES
    src/Components/AnitoMeshRenderer.cpp
    src/Components/AnitoMeshRenderer.h
    src/Components/AnitoCamera.cpp
    src/Components/AnitoCamera.h
    src/Components/AnitoTransform.cpp
    src/Components/AnitoTransform.h
)

# Window & Input
set(ANITO_WINDOW_SOURCES
    src/Window/AnitoWindow.cpp
    src/Window/AnitoWindow.h
    src/Input/AnitoInputManager.cpp
    src/Input/AnitoInputManager.h
)

# Main Application
set(ANITO_APP_SOURCES
    src/main.cpp
    src/AnitoEngine.cpp
    src/AnitoEngine.h
)

# Combine all sources
set(ANITO_ALL_SOURCES
    ${ANITO_MATH_SOURCES}
    ${ANITO_RENDERER_SOURCES}
    ${ANITO_DEFERRED_SOURCES}
    ${ANITO_LIGHTING_SOURCES}
    ${ANITO_GAMEOBJECT_SOURCES}
    ${ANITO_COMPONENT_SOURCES}
    ${ANITO_WINDOW_SOURCES}
    ${ANITO_APP_SOURCES}
)

# =============================================================================
# Executable Target
# =============================================================================
add_executable(AnitoEngine ${ANITO_ALL_SOURCES})

target_include_directories(AnitoEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${GLM_DIR}
)

# Link libraries
target_link_libraries(AnitoEngine PRIVATE
    bgfx
    bx
    bimg
    bimg_decode
    glfw
    BulletDynamics
    BulletCollision
    LinearMath
    imgui
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(AnitoEngine PRIVATE
        ANITO_PLATFORM_WINDOWS
        _CRT_SECURE_NO_WARNINGS
    )
endif()

if(MSVC)
    target_compile_options(AnitoEngine PRIVATE /W4 /WX-)
else()
    target_compile_options(AnitoEngine PRIVATE -Wall -Wextra)
endif()

# =============================================================================
# Post-build: Copy assets and shaders
# =============================================================================

# Find or build shaderc for shader compilation
set(SHADERC_EXECUTABLE "")

# Check multiple locations for shaderc
set(SHADERC_PATHS
    "${CMAKE_SOURCE_DIR}/external/bgfx/tools/shaderc.exe"
    "${CMAKE_SOURCE_DIR}/tools/shaderc/shaderc.exe"
    "${CMAKE_SOURCE_DIR}/external/bgfx/.build/win64_vs2022/bin/shadercRelease.exe"
    "${CMAKE_SOURCE_DIR}/external/bgfx/.build/win64_vs2019/bin/shadercRelease.exe"
    "${CMAKE_SOURCE_DIR}/external/bgfx/.build/win32_vs2022/bin/shadercRelease.exe"
)

foreach(SHADERC_PATH ${SHADERC_PATHS})
    if(EXISTS "${SHADERC_PATH}")
        set(SHADERC_EXECUTABLE "${SHADERC_PATH}")
        message(STATUS "Found shaderc at: ${SHADERC_EXECUTABLE}")
        break()
    endif()
endforeach()

# If shaderc is available, set up automatic shader compilation
if(SHADERC_EXECUTABLE)
    set(SHADER_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/assets/shaders/compiled")
    set(VARYING_DEF "${CMAKE_SOURCE_DIR}/assets/shaders/varying.def.sc")
    set(BGFX_INCLUDE "${CMAKE_SOURCE_DIR}/external/bgfx/src")

    # Create output directory
    file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

    # Function to compile a shader
    function(add_bgfx_shader SHADER_NAME SHADER_TYPE SHADER_SOURCE)
        if(SHADER_TYPE STREQUAL "VERTEX")
            set(TYPE_FLAG "vertex")
            set(PROFILE "s_5_0")
        elseif(SHADER_TYPE STREQUAL "FRAGMENT")
            set(TYPE_FLAG "fragment")
            set(PROFILE "s_5_0")
        else()
            message(FATAL_ERROR "Unknown shader type: ${SHADER_TYPE}")
        endif()

        set(OUTPUT_FILE "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.bin")

        add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND "${SHADERC_EXECUTABLE}"
                -f "${SHADER_SOURCE}"
                -o "${OUTPUT_FILE}"
                --type ${TYPE_FLAG}
                --platform windows
                -p ${PROFILE}
                --varyingdef "${VARYING_DEF}"
                -i "${BGFX_INCLUDE}"
            DEPENDS ${SHADER_SOURCE} ${VARYING_DEF}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Compiling ${SHADER_TYPE} shader: ${SHADER_NAME}"
            VERBATIM
        )

        # Add to parent scope
        set(${SHADER_NAME}_OUTPUT ${OUTPUT_FILE} PARENT_SCOPE)
    endfunction()

    # Compile shaders
    add_bgfx_shader(vs_simple VERTEX "${CMAKE_SOURCE_DIR}/assets/shaders/vs_simple.sc")
    add_bgfx_shader(fs_simple FRAGMENT "${CMAKE_SOURCE_DIR}/assets/shaders/fs_simple.sc")

    # Create a custom target that depends on all compiled shaders
    add_custom_target(CompileShaders ALL
        DEPENDS 
            ${vs_simple_OUTPUT}
            ${fs_simple_OUTPUT}
        COMMENT "Compiling all shaders"
    )

    # Make AnitoEngine depend on shader compilation
    add_dependencies(AnitoEngine CompileShaders)

    message(STATUS "Automatic shader compilation enabled")
    message(STATUS "  Shaders will be compiled to: ${SHADER_OUTPUT_DIR}")
else()
    message(WARNING "========================================")
    message(WARNING "shaderc.exe not found!")
    message(WARNING "")
    message(WARNING "Shaders will NOT be automatically compiled.")
    message(WARNING "")
    message(WARNING "To build shaderc from bgfx source:")
    message(WARNING "  Run: tools/build_shaderc.bat")
    message(WARNING "  This will build shaderc using bgfx's build system")
    message(WARNING "")
    message(WARNING "Then reconfigure CMake and rebuild.")
    message(WARNING "")
    message(WARNING "Or manually compile shaders using:")
    message(WARNING "  powershell -ExecutionPolicy Bypass -File tools/compile_shaders.ps1")
    message(WARNING "========================================")
endif()

add_custom_command(TARGET AnitoEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:AnitoEngine>/assets
    COMMENT "Copying assets to build directory"
)

message(STATUS "Anito Engine configuration complete")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
