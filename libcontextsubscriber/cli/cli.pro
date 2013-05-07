QT = core dbus
equals(QT_MAJOR_VERSION, 4): TARGET = context-listen
equals(QT_MAJOR_VERSION, 5): TARGET = context-listen5
SOURCES = context-listen.cpp \
          propertylistener.cpp \
          commandwatcher.cpp

HEADERS = propertylistener.h \
          commandwatcher.h

CONFIG += link_pkgconfig
equals(QT_MAJOR_VERSION, 4): PKGCONFIG += QJson

DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"context-listen\\\"

include(../../common/common.pri)

INCLUDEPATH += ../src
LIBS += -L../src -lcontextsubscriber

target.path = /usr/bin
INSTALLS += target
