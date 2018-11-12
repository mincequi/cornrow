TEMPLATE = subdirs

CONFIG += ordered
CONFIG += sdk_no_version_check

SUBDIRS += \
    ble \
    common \
    corocomo \
    #protocol \
    #zeroconf

ble.subdir              = ble
common.subdir           = common
corocomo.subdir		= corocomo
#protocol.subdir        = protocol
#zeroconf.subdir        = zeroconf

ble.depends = common
corocomo.depends = ble
