TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib c++11 create_prl

SOURCES += \
    ZeroconfWrapper.cpp

HEADERS += \
    ZeroconfWrapper.h

# tinysvcmdns
INCLUDEPATH += $$top_srcdir/thirdparty/tinysvcmdns/
LIBS    += -L$$top_srcdir/thirdparty/tinysvcmdns/ -ltinysvcmdns

# rpclib
INCLUDEPATH += $$top_srcdir/thirdparty/rpclib/dependencies/include/
LIBS    += -L$$top_srcdir/thirdparty/rpclib/build/ -lrpc
