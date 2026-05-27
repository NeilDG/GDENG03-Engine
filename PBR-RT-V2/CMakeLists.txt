cmake_minimum_required(VERSION 3.10)

if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("PBR-RT-V2" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(MSVC)
    add_compile_options(/wd5232 /wd4717)
endif()

set(DILIGENT_ENGINE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/external/DiligentEngine")
find_package(Git REQUIRED)
if(NOT EXISTS "${DILIGENT_ENGINE_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Cloning DiligentEngine with submodules into ${DILIGENT_ENGINE_SOURCE_DIR}")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} clone --recursive --depth 1 https://github.com/DiligentGraphics/DiligentEngine.git "${DILIGENT_ENGINE_SOURCE_DIR}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE DILIGENT_CLONE_RESULT
    )
    if(NOT DILIGENT_CLONE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to clone DiligentEngine recursively. Ensure git is installed and network access is available.")
    endif()
endif()

set(DILIGENT_ENGINE_CORE_CMAKE "${DILIGENT_ENGINE_SOURCE_DIR}/DiligentCore/CMakeLists.txt")
set(DILIGENT_ENGINE_TOOLS_CMAKE "${DILIGENT_ENGINE_SOURCE_DIR}/DiligentTools/CMakeLists.txt")
if(NOT EXISTS "${DILIGENT_ENGINE_CORE_CMAKE}" OR NOT EXISTS "${DILIGENT_ENGINE_TOOLS_CMAKE}")
    message(STATUS "DiligentEngine nested modules are missing. Initializing submodules recursively...")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} -C "${DILIGENT_ENGINE_SOURCE_DIR}" submodule update --init --recursive
        RESULT_VARIABLE DILIGENT_SUBMODULE_RESULT
    )
    if(NOT DILIGENT_SUBMODULE_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to initialize DiligentEngine submodules recursively.")
    endif()
endif()

set(DILIGENT_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_FX ON CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_SAMPLES ON CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(DILIGENT_BUILD_SAMPLE_BASE_ONLY ON CACHE BOOL "" FORCE)
set(DILIGENT_INSTALL_SAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${DILIGENT_ENGINE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/diligent-engine")

file(GLOB SPONZA_ASSETS LIST_DIRECTORIES false "${CMAKE_SOURCE_DIR}/assets/models/Sponza/glTF/*.*")
set(PBR_RT_V2_ASSETS
    ${SPONZA_ASSETS}
    assets/textures/papermill.ktx
    assets/textures/sheen_albedo_scaling.jpg
    assets/textures/charlie_preintegrated.jpg
)

set(PBR_RT_V2_SOURCES
    src/main.cpp
    src/AnitoApplication.cpp
    src/AnitoApplication.h
    src/AnitoPlatformWindow.h
    src/Renderer.cpp
    src/Renderer.h
    src/InputSystem.cpp
    src/InputSystem.h
    src/SurfelRendering/Surfel.h
    src/SurfelRendering/SurfelSceneBuilder.cpp
    src/SurfelRendering/SurfelSceneBuilder.h
    src/SurfelRendering/SurfelSpatialGrid.cpp
    src/SurfelRendering/SurfelSpatialGrid.h
    src/SurfelRendering/SurfelGatherPass.cpp
    src/SurfelRendering/SurfelGatherPass.h
    src/SurfelRendering/SurfelGIRenderPass.cpp
    src/SurfelRendering/SurfelGIRenderPass.h
    src/SurfelRendering/SurfelDebugView.cpp
    src/SurfelRendering/SurfelDebugView.h
    src/SurfelRendering/NightSceneLightingPreset.cpp
    src/SurfelRendering/NightSceneLightingPreset.h
)

add_sample_app(PBR-RT-V2
    IDE_FOLDER PBR-RT-V2
    SOURCES ${PBR_RT_V2_SOURCES}
    INCLUDES
        src/AnitoApplication.h
        src/AnitoPlatformWindow.h
        src/Renderer.h
        src/InputSystem.h
        src/SurfelRendering/Surfel.h
        src/SurfelRendering/SurfelSceneBuilder.h
        src/SurfelRendering/SurfelSpatialGrid.h
        src/SurfelRendering/SurfelGatherPass.h
        src/SurfelRendering/SurfelGIRenderPass.h
        src/SurfelRendering/SurfelDebugView.h
        src/SurfelRendering/NightSceneLightingPreset.h
    ASSETS ${PBR_RT_V2_ASSETS}
)

target_include_directories(PBR-RT-V2 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(PBR-RT-V2 PRIVATE
    Diligent-AssetLoader
    DiligentFX
)

target_compile_definitions(PBR-RT-V2 PRIVATE
    NOMINMAX
    UNICODE
    _UNICODE
)

if(MSVC)
    target_compile_options(PBR-RT-V2 PRIVATE /wd4996)
endif()

set_target_properties(PBR-RT-V2 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:PBR-RT-V2>"
)
