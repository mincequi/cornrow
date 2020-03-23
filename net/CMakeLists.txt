add_definitions(-DQZEROCONF_STATIC)

add_library(net STATIC
    include/net/NetService.h
    src/NetService.cpp

    ../thirdparty/QtZeroConf/qzeroconf.h
    ../thirdparty/QtZeroConf/avahi-qt/qt-watch.h
    ../thirdparty/QtZeroConf/avahi-qt/qt-watch_p.h

    ../thirdparty/QtZeroConf/avahi-qt/qt-watch.cpp
    ../thirdparty/QtZeroConf/avahiclient.cpp
    ../thirdparty/QtZeroConf/avahi-qt/qt-watch.cpp
)

#include(ExternalProject)
#ExternalProject_Get_Property(qtremoteobjects BINARY_DIR)
#include_directories(${BINARY_DIR}/include)
#link_directories(${BINARY_DIR}/lib)

target_include_directories(net
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    include/net

    ../thirdparty/QtZeroConf
)

target_link_libraries(net
    avahi-common
    avahi-client
    common
    cornrow
    Qt5::Network
)
