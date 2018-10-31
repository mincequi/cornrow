TEMPLATE = subdirs

INCLUDEPATH += $$top_srcdir

SUBDIRS += \
    common \
    #coroco \
    protocol \
    zeroconf

common.subdir           = common
#coroco.subdir           = coroco
protocol.subdir         = protocol
zeroconf.subdir         = zeroconf
#coroco.depends          = protocol
