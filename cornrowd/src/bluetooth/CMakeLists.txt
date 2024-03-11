find_package(Qt5 REQUIRED DBus Bluetooth)
find_package(KF5BluezQt)

add_library(bluetooth STATIC
    AudioSinkAgent.cpp
    Controller.cpp
)

target_link_libraries(bluetooth
    common
    cornrow
    Qt5::Bluetooth
    Qt5::DBus
    KF5::BluezQt
)
