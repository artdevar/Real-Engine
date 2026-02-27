set(TARGET ecs)

file(GLOB_RECURSE ECS_SRCS CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${TARGET} STATIC ${ECS_SRCS})

set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${TARGET}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(${TARGET} PUBLIC cxx_std_23)
target_link_libraries(${TARGET} PUBLIC common)