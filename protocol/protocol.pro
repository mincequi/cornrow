TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib c++11 create_prl

include(../cornrow.pri)

#RPCLIB
INCLUDEPATH += $$top_srcdir/thirdparty/rpclib/include/
LIBS    += -L$$top_srcdir/thirdparty/rpclib/build/ -lrpc

SOURCES += \
    ProtocolAdaptersV1.cpp \
    ProtocolTypesV1.cpp

HEADERS += \
    ProtocolAdaptersV1.h \
    ProtocolTypesV1.h

# common
LIBS += -L$$OUT_PWD/../common/ -lcommon
INCLUDEPATH += $$PWD/../common
DEPENDPATH += $$PWD/../common
PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a
