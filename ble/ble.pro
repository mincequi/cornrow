TEMPLATE = lib
QT     -= gui
CONFIG += staticlib create_prl

include(../cornrow.pri)

SOURCES += \
    #src/Converter.cpp \
    src/BleClient.cpp \
    src/Server.cpp \
    #src/ServerAdapter.cpp \
    src/ClientSession.cpp

HEADERS += \
    #include/ble/Converter.h \
    include/ble/BleClient.h \
    include/ble/Server.h \
    #include/ble/ServerAdapter.h \
    src/ClientSession.h

INCLUDEPATH += \
    include/ble

LIBS += -L$$OUT_PWD/../common/ -lcommon
