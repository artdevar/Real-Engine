cmake_minimum_required(VERSION 3.25.0)

set(TARGET tinygltf)
project(${TARGET} NONE)

file(GLOB TINYGLTF_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/tiny_gltf.cc
)

add_library(${TARGET} STATIC ${TINYGLTF_SRCS})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
if (NOT MSVC)
  target_compile_options(${TARGET} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-literal-operator>)
endif()