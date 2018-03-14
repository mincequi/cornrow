TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib c++11 create_prl

SOURCES += \
    Types.cpp \
    Util.cpp

HEADERS += \
    IControllable.h \
    Types.h \
    Util.h
