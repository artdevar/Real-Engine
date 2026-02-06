cmake_minimum_required(VERSION 3.25.0)
project(Real_Engine VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_executable(Real_Engine)

target_sources(Real_Engine PRIVATE
  src/Main.cpp
  src/tiny_gltf.cc
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
  src/ecs/Systems.cpp
  src/ecs/EntityManager.cpp
  src/ecs/ComponentsFactory.cpp
  src/editor/EditorUI.cpp
  src/utils/Logger.cpp
  src/utils/Resource.cpp
  src/glad/glad.c
  src/imgui/imgui.cpp
  src/imgui/imgui_draw.cpp
  src/imgui/imgui_widgets.cpp
  src/imgui/imgui_tables.cpp
  src/imgui/misc/cpp/imgui_stdlib.cpp
  src/imgui/backends/imgui_impl_glfw.cpp
  src/imgui/backends/imgui_impl_opengl3.cpp
)

if (WIN32)
  target_sources(Real_Engine PRIVATE src/editor/platform/SysUtils_Window.cpp)
elseif (UNIX AND NOT APPLE)
  target_sources(Real_Engine PRIVATE src/editor/platform/SysUtils_Unix.cpp)
endif ()

find_package(OpenGL REQUIRED)

link_directories(${PROJECT_SOURCE_DIR}/lib)

target_link_libraries(Real_Engine glfw)
target_link_libraries(Real_Engine assimp)
target_link_libraries(Real_Engine tbb)

target_compile_definitions(Real_Engine PRIVATE ENABLE_EDITOR=1)

target_include_directories(Real_Engine
  PUBLIC ${PROJECT_SOURCE_DIR}/lib
  PUBLIC ${PROJECT_SOURCE_DIR}/lib/ctti
  PUBLIC ${PROJECT_SOURCE_DIR}/lib/assimp/include
  PUBLIC ${PROJECT_SOURCE_DIR}/lib/glm-1.0.1
  PUBLIC ${PROJECT_SOURCE_DIR}/src
  PUBLIC ${PROJECT_SOURCE_DIR}/src/imgui
)
