QT = core dbus
equals(QT_MAJOR_VERSION, 4): TARGET = context-ls
equals(QT_MAJOR_VERSION, 5): TARGET = context-ls5

SOURCES = context-ls.cpp

DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"context-ls\\\"

include(../../common/common.pri)

INCLUDEPATH += ../src
LIBS += -L../src -lcontextsubscriber

target.path = /usr/bin
INSTALLS += target
