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
    src/TcpClient.cpp

HEADERS += \
    include/net/TcpClient.h

INCLUDEPATH += \
    include/net

LIBS += -L$$OUT_PWD/../common/ -lcommon
