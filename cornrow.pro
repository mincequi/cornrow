TEMPLATE = subdirs

SUBDIRS += \
    common \
    corocomo \
    qtzeroprops

common.subdir   = common
corocomo.subdir = corocomo
qtzeroprops.subdir = thirdparty/qtzeroprops

corocomo.depends = common qtzeroprops
