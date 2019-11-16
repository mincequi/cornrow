project(bluebricated)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 CONFIG REQUIRED Bluetooth Core DBus)

add_executable(${PROJECT_NAME}
    src/Controller.cpp
    src/main.cpp
    src/audio/Controller.cpp
    src/bluetooth/AudioSinkAgent.cpp
    src/bluetooth/Controller.cpp
    ../cornrowd/src/audio/Converter.cpp
    ../cornrowd/src/audio/FileDescriptorSource.cpp
    ../cornrowd/src/audio/Pipeline.cpp
    ../cornrowd/src/audio/CoroPipeline.cpp
)

target_link_libraries(${PROJECT_NAME}
    audio
    gstreamermm-dsp
    KF5BluezQt
    Qt5::Bluetooth
    Qt5::DBus
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
