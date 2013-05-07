TEMPLATE = subdirs
SUBDIRS = \
    libcontextprovider \
    libcontextsubscriber \
    context-provide

equals(QT_MAJOR_VERSION, 4) {
    SUBDIRS += python \
               spec
}

context-provide.depends = libcontextprovider libcontextsubscriber

