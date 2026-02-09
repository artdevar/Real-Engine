cmake_minimum_required(VERSION 3.25.0)

set(TARGET imgui)
project(${TARGET} NONE)

file(GLOB IMGUI_CORE
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
file(GLOB IMGUI_MISC
	${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp/*.cpp
)

set(IMGUI_SRCS ${IMGUI_CORE} ${IMGUI_MISC})
list(APPEND IMGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
list(APPEND IMGUI_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

add_library(${TARGET} STATIC ${IMGUI_SRCS})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)