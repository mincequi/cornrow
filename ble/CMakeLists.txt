find_package(Qt5Bluetooth REQUIRED)

add_library(ble STATIC
    include/ble/Central.h
    include/ble/Peripheral.h
    src/Central.cpp
    src/PeqAdapter.cpp
    src/Peripheral.cpp
)

target_include_directories(ble
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    include/ble
)

target_link_libraries(ble
    Qt5Bluetooth
    Qt5::Core
)

add_subdirectory(tests)
