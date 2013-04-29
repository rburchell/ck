QT = core dbus xml
TARGET = context-provide

CONFIG += link_pkgconfig
PKGCONFIG += QJson

DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"context-provide\\\"

include(../common/common.pri)

INCLUDEPATH += ../libcontextprovider/src ../libcontextsubscriber/src/

LIBS += -L../libcontextprovider/src -lcontextprovider
LIBS += -L../libcontextsubscriber/src -lcontextsubscriber


SOURCES = context-provide.cpp \
          commandwatcher.cpp \
          propertyproxy.cpp

HEADERS = commandwatcher.h propertyproxy.h

target.path = /usr/bin
INSTALLS += target
