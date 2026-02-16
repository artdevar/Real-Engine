cmake_minimum_required(VERSION 3.25.0)

set(TARGET glad)
project(${TARGET} NONE)

file(GLOB GLAD_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/glad.c
)

add_library(${TARGET} STATIC ${GLAD_SRCS})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)