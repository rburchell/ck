QT = core dbus xml
equals(QT_MAJOR_VERSION, 4): TARGET = context-provide
equals(QT_MAJOR_VERSION, 5): TARGET = context-provide5

CONFIG += link_pkgconfig
equals(QT_MAJOR_VERSION, 4): PKGCONFIG += QJson

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
