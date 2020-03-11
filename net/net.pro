TEMPLATE = lib
QT     -= gui
QT     += core network
CONFIG += staticlib create_prl

include(../cornrow.pri)

DEFINES += QZEROCONF_STATIC
include(../thirdparty/qtzeroconf/qtzeroconf.pri)

SOURCES += \
    src/NetClient.cpp

HEADERS += \
    include/net/NetClient.h

INCLUDEPATH += \
    include/net

LIBS += -L$$OUT_PWD/../common/ -lcommon
