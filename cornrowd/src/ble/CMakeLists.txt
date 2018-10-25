find_package(Qt5Bluetooth REQUIRED)

add_library(ble STATIC
    PeqAdapter.cpp
    Controller.cpp
)

target_link_libraries(ble
    Qt5Bluetooth
    Qt5::Core
)
