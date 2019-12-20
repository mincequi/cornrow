TEMPLATE = lib
QT     -= gui
CONFIG += staticlib create_prl

include(../cornrow.pri)

SOURCES += \
    src/Util.cpp \
    src/Types.cpp \
    src/ble/Converter.cpp

HEADERS += \
    include/common/IControllable.h \
    include/common/Util.h \
    include/common/Types.h \
    include/common/ble/Converter.h

INCLUDEPATH += \
    include/ \
    include/common \
    include/common/ble
