add_library(common STATIC
    include/common/RemoteDataStore.h
    src/RemoteDataStore.cpp
    src/Types.cpp
    src/Util.cpp
    src/ble/Converter.cpp
)

target_include_directories(common
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    include/common
    include/common/ble
)

# @TODO(mawe): remove qt dependency
target_link_libraries(common
    Qt5::Core
)
