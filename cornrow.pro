TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
    ble \
    common \
    corocomo \
    #protocol \
    #zeroconf

common.subdir           = common
corocomo.subdir		= corocomo
#protocol.subdir        = protocol
#zeroconf.subdir        = zeroconf

ble.depends = common
corocomo.depends = ble
