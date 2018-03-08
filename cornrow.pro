TEMPLATE = subdirs

INCLUDEPATH += $$top_srcdir

SUBDIRS += \
    common \
    cornrowd \
    gstreamermm-dsp \
    protocol

common.subdir           = common
gstreamermm-dsp.subdir  = gstreamermm-dsp
protocol.subdir         = protocol

cornrowd.depends        = common gstreamermm-dsp protocol
