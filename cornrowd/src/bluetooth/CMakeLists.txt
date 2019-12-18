find_package(Qt5Bluetooth REQUIRED)

add_library(bluetooth STATIC
    AudioSinkAgent.cpp
    Controller.cpp
)

target_link_libraries(bluetooth
    common
    cornrow
    KF5BluezQt
    Qt5::Bluetooth
    Qt5::DBus
)
