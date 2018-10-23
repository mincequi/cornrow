TEMPLATE = subdirs

INCLUDEPATH += $$top_srcdir

SUBDIRS += \
    common \
    cornrowd \
    coroco \
    corocomo \
    protocol \
    zeroconf

common.subdir           = common
coroco.subdir           = coroco
corocomo.subdir         = corocomo
gstreamermm-dsp.subdir  = gstreamermm-dsp
protocol.subdir         = protocol
zeroconf.subdir         = zeroconf

cornrowd.depends        = common gstreamermm-dsp protocol zeroconf
coroco.depends          = protocol
