QT  += core gui widgets

TARGET  = coroco
TEMPLATE = app

CONFIG += c++11 link_prl

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += ASIO_STANDALONE

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../cornrow.pri)

SOURCES += main.cpp\
    MainWindow.cpp \
    TableSpinBox.cpp \
    Types.cpp \
    ZeroconfBonjour.cpp

HEADERS += MainWindow.h \
    TableSpinBox.h \
    Types.h \
    ZeroconfBonjour.h

FORMS   += MainWindow.ui

# kplotting
INCLUDEPATH += $$top_srcdir/thirdparty/kplotting/src
INCLUDEPATH += $$top_srcdir/thirdparty/kplotting/build/src/
LIBS += -L$$top_srcdir/thirdparty/kplotting/build/bin/ -lKF5Plotting

# rpclib
INCLUDEPATH += $$top_srcdir/thirdparty/rpclib/include/
INCLUDEPATH += $$top_srcdir/thirdparty/rpclib/dependencies/include/

# protocol
LIBS += -L$$OUT_PWD/../protocol/ -lprotocol
PRE_TARGETDEPS += $$OUT_PWD/../protocol/libprotocol.a

macx {
QMAKE_LFLAGS += -F /System/Library/Frameworks/CoreServices.framework/
LIBS+= -framework CoreServices
}

