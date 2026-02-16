cmake_minimum_required(VERSION 3.25)

project(Real_Engine VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options

option(DEV_STAGE "Enable development-stage features" ON)

if (DEV_STAGE)
    set(DEV_STAGE_VAL 1)
    set(SHADERS_HOT_RELOAD_VAL 1)
else()
    set(DEV_STAGE_VAL 0)
    set(SHADERS_HOT_RELOAD_VAL 0)
endif()

# Exec

add_executable(Real_Engine)

target_sources(Real_Engine PRIVATE
    src/Main.cpp
    src/engine/Engine.cpp
    src/engine/Display.cpp
    src/engine/InputManager.cpp
    src/engine/Camera.cpp
    src/engine/ResourceManager.cpp
    src/scenes/World.cpp
    src/graphics/GLRenderer.cpp
    src/graphics/Texture.cpp
    src/graphics/Shader.cpp
    src/graphics/Model.cpp
    src/graphics/TinyGLTFParseStrategy.cpp
    src/ecs/systems/WorldRenderSystem.cpp
    src/ecs/systems/SkyboxRenderSystem.cpp
    src/ecs/systems/PhysicsSystem.cpp
    src/ecs/systems/LightingSystem.cpp
    src/ecs/systems/ShadowRenderSystem.cpp
    src/ecs/factories/ModelComponentFactory.cpp
    src/ecs/factories/SkyboxComponentFactory.cpp
    src/ecs/EntityManager.cpp
    src/ecs/ComponentsFactory.cpp
    src/editor/EditorUI.cpp
    src/utils/Logger.cpp
    src/utils/Resource.cpp
    src/utils/Path.cpp
)

if (WIN32)
    target_sources(Real_Engine PRIVATE
        src/editor/platform/SysUtils_Windows.cpp
    )
elseif (UNIX AND NOT APPLE)
    target_sources(Real_Engine PRIVATE
        src/editor/platform/SysUtils_Unix.cpp
    )
endif()

# Definitions

target_compile_definitions(Real_Engine PRIVATE
    GLM_ENABLE_EXPERIMENTAL
    DEV_STAGE=${DEV_STAGE_VAL}
    SHADERS_HOT_RELOAD=${SHADERS_HOT_RELOAD_VAL}
    ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/"
    SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
)

# Includes

target_include_directories(Real_Engine PUBLIC
    src
    lib/
    lib/ctti
    lib/glad
    lib/glm
    lib/tinygltf
    lib/MikkTSpace
    lib/imgui
)

# Dependencies

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

link_directories(lib)
add_subdirectory(lib/glad)
add_subdirectory(lib/glm)
add_subdirectory(lib/tinygltf)
add_subdirectory(lib/MikkTSpace)
add_subdirectory(lib/imgui)

# Linking

target_link_libraries(Real_Engine PRIVATE
    glfw
    glad
    tinygltf
    mikktspace
    imgui
)

if (WIN32)
    target_link_libraries(Real_Engine PRIVATE
        opengl32
    )
endif()

# PCH

target_precompile_headers(Real_Engine PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>
)
