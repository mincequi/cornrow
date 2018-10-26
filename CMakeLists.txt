cmake_minimum_required(VERSION 3.10)

project(cornrow)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)

add_subdirectory(ble)
add_subdirectory(cornrowd)
add_subdirectory(thirdparty/bluez-qt)
add_subdirectory(thirdparty/gstreamermm-dsp)
