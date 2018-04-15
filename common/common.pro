TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib c++11 create_prl

SOURCES += \
    Util.cpp \
    Types.cpp

HEADERS += \
    IControllable.h \
    Util.h \
    Types.h
