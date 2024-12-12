add_library(common STATIC
    src/Types.cpp
    src/Util.cpp
    src/ble/Converter.cpp
)

target_include_directories(common
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    include/common
    include/common/ble
)
