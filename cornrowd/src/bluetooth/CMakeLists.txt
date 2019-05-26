add_library(bluetooth STATIC
    AudioSinkAgent.cpp
    Controller.cpp
)

target_link_libraries(bluetooth
    ble
    KF5BluezQt
    Qt5::DBus
)
