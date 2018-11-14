TEMPLATE = subdirs

SUBDIRS += \
    ble \
    common \
    corocomo

ble.subdir      = ble
common.subdir   = common
corocomo.subdir = corocomo

ble.depends = common
corocomo.depends = ble
