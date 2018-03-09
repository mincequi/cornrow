TEMPLATE = app
CONFIG -= qt app_bundle
CONFIG += console c++11 link_prl

include(../cornrow.pri)

SOURCES += main.cpp

HEADERS +=

#
LIBS += -pthread

# glibmm
CONFIG += link_pkgconfig
PKGCONFIG += glibmm-2.4

# rpclib
INCLUDEPATH += $$top_srcdir/thirdparty/rpclib/include/

# gstreamer-dsp
LIBS += -L$$OUT_PWD/../gstreamermm-dsp/ -lgstreamermm-dsp
INCLUDEPATH += $$PWD/../gstreamermm-dsp
DEPENDPATH += $$PWD/../gstreamermm-dsp
PRE_TARGETDEPS += $$OUT_PWD/../gstreamermm-dsp/libgstreamermm-dsp.a

# protocol
LIBS += -L$$OUT_PWD/../protocol/ -lprotocol
INCLUDEPATH += $$PWD/../protocol
DEPENDPATH += $$PWD/../protocol
PRE_TARGETDEPS += $$OUT_PWD/../protocol/libprotocol.a

# zeroconf
LIBS += -L$$OUT_PWD/../zeroconf/ -lzeroconf
INCLUDEPATH += $$PWD/../zeroconf
DEPENDPATH += $$PWD/../zeroconf
PRE_TARGETDEPS += $$OUT_PWD/../zeroconf/libzeroconf.a
