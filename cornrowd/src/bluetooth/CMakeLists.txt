add_library(bluetooth STATIC
    AudioSinkAgent.cpp
    Controller.cpp
)

target_link_libraries(bluetooth
    KF5BluezQt
    Qt5DBus
)
