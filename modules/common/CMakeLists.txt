set(TARGET common)

add_library(${TARGET} INTERFACE)

target_include_directories(${TARGET} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/modules>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(${TARGET} INTERFACE cxx_std_23)

set_target_properties(${TARGET} PROPERTIES EXPORT_NAME common)
