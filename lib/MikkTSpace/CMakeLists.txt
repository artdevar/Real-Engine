cmake_minimum_required(VERSION 3.25.0)

set(TARGET mikktspace)
project(${TARGET} NONE)

file(GLOB MIKKTSRCS
	${CMAKE_CURRENT_SOURCE_DIR}/mikktspace.c
)

add_library(${TARGET} STATIC ${MIKKTSRCS})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)