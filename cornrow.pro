TEMPLATE = subdirs

SUBDIRS += \
    common \
    net \
    corocomo

common.subdir   = common
corocomo.subdir = corocomo
net.subdir      = net

net.depends = common
corocomo.depends = net
