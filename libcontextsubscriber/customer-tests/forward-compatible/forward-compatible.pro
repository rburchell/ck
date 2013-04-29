QT = core dbus
TARGET = check-version

SOURCES = check-version.cpp

INCLUDEPATH += ../../src
LIBS += -L../../src -lcontextsubscriber

target.path = /usr/bin
INSTALLS += target

