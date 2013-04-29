TEMPLATE = subdirs
SUBDIRS = \
    libcontextprovider \
    libcontextsubscriber \
    context-provide \
    spec \
    python

context-provide.depends = libcontextprovider libcontextsubscriber

