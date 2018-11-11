TEMPLATE = lib
QT     -= gui
CONFIG += staticlib c++11 create_prl

include(../cornrow.pri)

SOURCES += \
    src/Central.cpp \
    src/Converter.cpp \
    src/Peripheral.cpp \
    src/PeripheralAdapter.cpp

HEADERS += \
    include/ble/Central.h \
    include/ble/Peripheral.h \
    include/ble/PeripheralAdapter.h

INCLUDEPATH += \
    include/ble

LIBS += -L$$OUT_PWD/../common/ -lcommon
