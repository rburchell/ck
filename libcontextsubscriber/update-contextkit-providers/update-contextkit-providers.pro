QT = core xml dbus
TARGET = update-contextkit-providers

SOURCES = update-contextkit-providers.cpp

INCLUDEPATH += ../src
LIBS += -L../src -lcontextsubscriber

DEFINES += DEFAULT_CONTEXT_PROVIDERS=\\\"/usr/share/contextkit/providers/\\\"
DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"update-contextkit-providers\\\"

target.path = /usr/bin
INSTALLS += target
