cmake_minimum_required(VERSION 3.10)

project(cornrowd)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)

add_subdirectory(src/audio)
add_subdirectory(src/bluetooth)
add_subdirectory(tests)

add_executable(${PROJECT_NAME}
    src/Config.cpp
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
    audio
    ble
    bluetooth
)
