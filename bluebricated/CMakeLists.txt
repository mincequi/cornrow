project(bluebricated)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Bluetooth Core DBus)

add_executable(${PROJECT_NAME}
    src/Controller.cpp
    src/main.cpp
    src/audio/Controller.cpp
    src/audio/Pipeline.cpp
    src/bluetooth/AudioSinkAgent.cpp
    src/bluetooth/Controller.cpp
    ../cornrowd/src/audio/FileDescriptorSource.cpp
)

target_link_libraries(${PROJECT_NAME}
    common
    gstreamermm-dsp
    KF5BluezQt
    Qt5::Bluetooth
    Qt5::DBus
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
