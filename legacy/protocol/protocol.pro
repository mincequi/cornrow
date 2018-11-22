TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib c++11 create_prl object_parallel_to_source

include(../cornrow.pri)

SOURCES += \
    v1/ServerAdapter.cpp \
    v1/ClientAdapter.cpp \
    v1/Converter.cpp \
    v2/ClientAdapter.cpp \
    v2/ServerAdapter.cpp \
    v2/Converter.cpp

HEADERS += \
    v1/ClientAdapter.h \
    v1/ServerAdapter.h \
    v1/Converter.h \
    v1/Types.h \
    v2/ClientAdapter.h \
    v2/Types.h \
    v2/ServerAdapter.h \
    v2/Converter.h

# common
LIBS += -L$$OUT_PWD/../common/ -lcommon
INCLUDEPATH += $$PWD/../common/include
DEPENDPATH += $$PWD/../common
PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a

# rpclib
INCLUDEPATH += $$top_srcdir/thirdparty/rpclib/include/
LIBS    += -L$$top_srcdir/thirdparty/rpclib/build/ -lrpc
