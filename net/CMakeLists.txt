add_definitions(-DQZEROCONF_STATIC)

add_library(net STATIC
    include/QtZeroProps/QZeroPropsServer.h
    include/QtZeroProps/QZeroPropsService.h

    #src/QZeroPropsClient.cpp
    src/QZeroPropsServer.cpp
    src/QZeroPropsServerPrivate.cpp
    src/QZeroPropsService.cpp
    src/QZeroPropsServicePrivate.cpp
    src/QZeroPropsWsServer.h
    src/QZeroPropsWsServer.cpp
    src/QZeroPropsWsService.h
    src/QZeroPropsWsService.cpp

    ../thirdparty/QtZeroConf/qzeroconf.h
    ../thirdparty/QtZeroConf/avahi-qt/qt-watch.h
    ../thirdparty/QtZeroConf/avahi-qt/qt-watch_p.h

    ../thirdparty/QtZeroConf/avahi-qt/qt-watch.cpp
    ../thirdparty/QtZeroConf/avahiclient.cpp
    ../thirdparty/QtZeroConf/avahi-qt/qt-watch.cpp

    ../thirdparty/qmsgpack/src/msgpack.cpp
    ../thirdparty/qmsgpack/src/msgpackcommon.cpp
    ../thirdparty/qmsgpack/src/msgpackstream.cpp
    ../thirdparty/qmsgpack/src/private/pack_p.cpp
    ../thirdparty/qmsgpack/src/private/unpack_p.cpp
    ../thirdparty/qmsgpack/src/private/qt_types_p.cpp
    ../thirdparty/qmsgpack/src/stream/time.cpp
    ../thirdparty/qmsgpack/src/stream/geometry.cpp

    ../thirdparty/qmsgpack/src/msgpack.h
    ../thirdparty/qmsgpack/src/endianhelper.h
    ../thirdparty/qmsgpack/src/msgpackcommon.h
    ../thirdparty/qmsgpack/src/msgpack_export.h
    ../thirdparty/qmsgpack/src/msgpackstream.h
    ../thirdparty/qmsgpack/src/private/pack_p.h
    ../thirdparty/qmsgpack/src/private/unpack_p.h
    ../thirdparty/qmsgpack/src/private/qt_types_p.h
    ../thirdparty/qmsgpack/src/stream/time.h
    ../thirdparty/qmsgpack/src/stream/geometry.h
)

target_include_directories(net
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    src # @TODO(mawe): temporary hack
PRIVATE
    include/net
    ../thirdparty/QtZeroConf
    ../thirdparty/qmsgpack/src
)

target_link_libraries(net
    avahi-common
    avahi-client
    common
    cornrow
    Qt5::WebSockets
)
