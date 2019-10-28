TEMPLATE = lib
QT     -= gui
CONFIG += staticlib c++11 create_prl

include(../cornrow.pri)

SOURCES += \
    #src/Converter.cpp \
    src/Client.cpp \
    src/Server.cpp \
    #src/ServerAdapter.cpp \
    src/ClientSession.cpp

HEADERS += \
    include/ble/Client.h \
    #include/ble/Converter.h \
    include/ble/Server.h \
    #include/ble/ServerAdapter.h \
    src/ClientSession.h

INCLUDEPATH += \
    include/ble

LIBS += -L$$OUT_PWD/../common/ -lcommon
