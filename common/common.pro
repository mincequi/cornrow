TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib c++11 create_prl

SOURCES += \
    src/Util.cpp \
    src/Types.cpp

HEADERS += \
    include/common/IControllable.h \
    include/common/Util.h \
    include/common/Types.h

INCLUDEPATH += \
    include/common
