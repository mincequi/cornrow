TEMPLATE = lib
CONFIG -= qt
CONFIG += staticlib c++11 create_prl

include(../cornrow.pri)

SOURCES += \
    GstDspBiquad.cpp \
    GstDspCrossover.cpp \
    GstDspPlugin.cpp \
    GstDspLoudness.cpp \
    GstDspPeq.cpp \
    GstDspWrapper.cpp

HEADERS += \
    GstDspBiquad.h \
    GstDspCrossover.h \
    GstDspPlugin.h \
    GstDspLoudness.h \
    GstDspPeq.h \
    GstDspWrapper.h

CONFIG += link_pkgconfig
PKGCONFIG += gstreamermm-1.0
