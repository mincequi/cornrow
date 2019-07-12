find_package(Qt5Bluetooth REQUIRED)

add_library(ble STATIC
    include/ble/Client.h
    include/ble/Converter.h
    #include/ble/Server.h
    #include/ble/ServerAdapter.h
    src/Client.cpp
    src/ClientSession.cpp
    src/ClientSession.h
    src/Converter.cpp
    #src/Server.cpp
    #src/ServerAdapter.cpp
    #src/ServerSession.cpp
    #src/ServerSession.h
)

target_include_directories(ble
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    include/ble
)

target_link_libraries(ble
    common
    Qt5::Bluetooth
    Qt5::Core
)

add_subdirectory(tests)
