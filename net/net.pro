TEMPLATE = lib
QT     -= gui
QT     += core bluetooth websockets
CONFIG += staticlib create_prl

DEFINES += QZEROCONF_STATIC MSGPACK_NO_PACKTYPES
include(../thirdparty/QtZeroConf/qtzeroconf.pri)
include(../thirdparty/qmsgpack/qmsgpack.pri)

SOURCES += \
    src/BleClientSession.cpp \
    src/BleServer.cpp \
    src/BleServerSession.cpp \
    src/QZeroPropsBleClient.cpp \
    src/QZeroPropsClient.cpp \
    src/QZeroPropsService.cpp \
    src/QZeroPropsServicePrivate.cpp \
    src/QZeroPropsWsService.cpp

HEADERS += \
    include/QtZeroProps/QZeroPropsTypes.h \
    include/QtZeroProps/QZeroPropsClient.h \
    include/QtZeroProps/QZeroPropsService.h \
    src/BleClientSession.h \
    src/BleServerSession.h \
    src/QZeroPropsBleClient.h \
    src/QZeroPropsBleService.h \
    src/QZeroPropsServicePrivate.h \
    src/QZeroPropsWsService.h

INCLUDEPATH += \
    include
