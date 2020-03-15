TEMPLATE = subdirs

SUBDIRS += \
    common \
    ble \
    net \
    corocomo

ble.subdir      = ble
common.subdir   = common
corocomo.subdir = corocomo
net.subdir      = net

ble.depends = common
net.depends = common
corocomo.depends = ble net
