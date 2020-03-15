TEMPLATE = lib
QT     -= gui
# @TODO(mawe): remove bluetooth dependency
QT     += core network bluetooth
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
