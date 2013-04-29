QT = core dbus
TARGET = context-listen
SOURCES = context-listen.cpp \
          propertylistener.cpp \
          commandwatcher.cpp

HEADERS = propertylistener.h \
          commandwatcher.h

CONFIG += link_pkgconfig
PKGCONFIG += QJson

DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"context-listen\\\"

include(../../common/common.pri)

INCLUDEPATH += ../src
LIBS += -L../src -lcontextsubscriber

target.path = /usr/bin
INSTALLS += target
