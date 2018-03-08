TEMPLATE = app
CONFIG -= qt app_bundle
CONFIG += console c++11 link_prl

include(../cornrow.pri)

SOURCES += main.cpp

HEADERS +=

# glibmm
CONFIG += link_pkgconfig
PKGCONFIG += glibmm-2.4

#
LIBS += -pthread

# rpclib
INCLUDEPATH += $$top_srcdir/thirdparty/rpclib/include/

# gstreamer-dsp
LIBS += -L$$OUT_PWD/../common/gstreamermm-dsp/ -lgstreamermm-dsp
INCLUDEPATH += $$PWD/../common/gstreamermm-dsp
DEPENDPATH += $$PWD/../common/gstreamermm-dsp
PRE_TARGETDEPS += $$OUT_PWD/../common/gstreamermm-dsp/libgstreamermm-dsp.a

# protocol
LIBS += -L$$OUT_PWD/../common/protocol/ -lprotocol
INCLUDEPATH += $$PWD/../common/protocol
DEPENDPATH += $$PWD/../common/protocol
PRE_TARGETDEPS += $$OUT_PWD/../common/protocol/libprotocol.a
