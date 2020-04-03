TEMPLATE = lib
QT     -= gui
# @TODO(mawe): remove bluetooth dependency
QT     += core network bluetooth websockets
CONFIG += staticlib create_prl

include(../cornrow.pri)

DEFINES += QZEROCONF_STATIC
include(../thirdparty/QtZeroConf/qtzeroconf.pri)
include(../thirdparty/qmsgpack/qmsgpack.pri)

SOURCES += \
    src/BleCentralAdapter.cpp \
    src/BleClient.cpp \
    src/BleClientSession.cpp \
    src/BleServer.cpp \
    src/BleServerSession.cpp \
    src/QZeroPropsClient.cpp \
    src/QZeroPropsService.cpp \
    src/QZeroPropsServicePrivate.cpp \
    src/QZeroPropsWebSocketService.cpp

HEADERS += \
    include/QZeroProps/QZeroPropsTypes.h \
    include/QZeroProps/QZeroPropsClient.h \
    include/QZeroProps/QZeroPropsService.h \
    src/BleClientSession.h \
    src/BleServerSession.h \
    src/QZeroPropsServicePrivate.h \
    src/QZeroPropsWebSocketService.h \
    src/Defines.h

INCLUDEPATH += \
    include

#LIBS += -L$$OUT_PWD/../common/ -lcommon
