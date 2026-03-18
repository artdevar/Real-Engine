cmake_minimum_required(VERSION 3.25)

project(Real_Engine VERSION 0.0.1 LANGUAGES CXX)

# Options

option(DEV_STAGE "Enable development-stage features" ON)

if (DEV_STAGE)
    set(DEV_STAGE_VAL 1)
    set(SHADERS_HOT_RELOAD_VAL 1)
else()
    set(DEV_STAGE_VAL 0)
    set(SHADERS_HOT_RELOAD_VAL 0)
endif()

set(PROJECT_NAME "Real Engine")
set(ASSETS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(APP_ICON ${ASSETS_DIRECTORY}/icons/icon.png)

set(CMAKE_CXX_EXTENSIONS OFF)

# Exec

if(WIN32)
    set(APP_RC ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/windows/app.rc)
    add_executable(Real_Engine ${APP_RC})
    source_group("resources" FILES ${APP_RC})
else()
    add_executable(Real_Engine)
endif()

file(GLOB_RECURSE SHADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.*)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

if (WIN32)
    list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*SysUtils_Unix.cpp$")
elseif (UNIX)
    list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*SysUtils_Windows.cpp$")
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/shaders PREFIX "shaders" FILES ${SHADERS})
target_sources(Real_Engine PRIVATE ${SHADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${PROJECT_SOURCES})
target_sources(Real_Engine PRIVATE ${PROJECT_SOURCES})

# Definitions

target_compile_definitions(Real_Engine PRIVATE
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_PURE
    APP_NAME="${PROJECT_NAME}"
	  APP_ICON="${APP_ICON}"
    DEV_STAGE=${DEV_STAGE_VAL}
    SHADERS_HOT_RELOAD=${SHADERS_HOT_RELOAD_VAL}
    PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/"
    ASSETS_DIR="${ASSETS_DIRECTORY}"
    SHADERS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
)

target_compile_options(Real_Engine PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/GR->
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fno-rtti>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wnon-virtual-dtor>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Woverloaded-virtual>
)

target_compile_features(Real_Engine PRIVATE cxx_std_23)

# Includes

target_include_directories(Real_Engine PUBLIC
    src
    lib/
    lib/glm
    modules/
)

# Dependencies

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

add_subdirectory(lib/glad)
add_subdirectory(lib/glm)
add_subdirectory(lib/tinygltf)
add_subdirectory(lib/MikkTSpace)
add_subdirectory(lib/imgui)
add_subdirectory(modules/common)
add_subdirectory(modules/ecs)
add_subdirectory(modules/events)

# Linking

target_link_libraries(Real_Engine PRIVATE
    glfw
    glad
    tinygltf
    mikktspace
    imgui
    common
    ecs
    events
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
