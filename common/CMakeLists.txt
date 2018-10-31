add_library(common STATIC
    src/Types.cpp
    src/Util.cpp
)

target_include_directories(common
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    include/common
)
