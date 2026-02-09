cmake_minimum_required(VERSION 3.25.0)

set(TARGET Real_Engine)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/")

project(${TARGET} VERSION 0.0.1)
add_executable(${TARGET})

target_compile_definitions(${TARGET} PUBLIC GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(${TARGET} PRIVATE EDITOR_ENABLED=1)
target_compile_definitions(${TARGET} PRIVATE SHADERS_HOT_RELOAD=1)
target_compile_definitions(${TARGET} PRIVATE ASSETS_DIR="${ASSETS_DIR}")

target_sources(${TARGET} PRIVATE
  src/Main.cpp
  src/engine/Config.cpp
  src/engine/Engine.cpp
  src/engine/Display.cpp
  src/engine/InputManager.cpp
  src/engine/Camera.cpp
  src/engine/ResourceManager.cpp
  src/scenes/World.cpp
  src/graphics/Renderer.cpp
  src/graphics/Shader.cpp
  src/graphics/Model.cpp
  src/graphics/TinyGLTFParseStrategy.cpp
  src/ecs/systems/WorldRenderSystem.cpp
  src/ecs/systems/SkyboxRenderSystem.cpp
  src/ecs/systems/PhysicsSystem.cpp
  src/ecs/systems/LightingSystem.cpp
  src/ecs/systems/ShadowRenderSystem.cpp
  src/ecs/EntityManager.cpp
  src/ecs/ComponentsFactory.cpp
  src/editor/EditorUI.cpp
  src/utils/Logger.cpp
  src/utils/Resource.cpp
)

if (WIN32)
  target_sources(${TARGET} PRIVATE src/editor/platform/SysUtils_Window.cpp)
elseif (UNIX AND NOT APPLE)
  target_sources(${TARGET} PRIVATE src/editor/platform/SysUtils_Unix.cpp)
endif ()

#find_package(OpenGL REQUIRED)
#find_package(glfw3 REQUIRED)

link_directories(lib)
include_directories(lib)
include_directories(lib/ctti)
include_directories(lib/glm-1.0.1)
include_directories(lib/tinygltf)
include_directories(lib/imgui)
include_directories(src)

add_subdirectory(lib/imgui)
add_subdirectory(lib/tinygltf)
add_subdirectory(lib/glad)

target_link_libraries(${TARGET} PRIVATE glad)
target_link_libraries(${TARGET} PRIVATE glfw)
target_link_libraries(${TARGET} PRIVATE tinygltf)
target_link_libraries(${TARGET} PRIVATE imgui)

target_precompile_headers(${TARGET} PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_SOURCE_DIR}/src/pch.h>
)
