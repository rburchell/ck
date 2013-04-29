QT = core dbus
TARGET = context-ls

SOURCES = context-ls.cpp

DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"context-ls\\\"

include(../../common/common.pri)

INCLUDEPATH += ../src
LIBS += -L../src -lcontextsubscriber

target.path = /usr/bin
INSTALLS += target
