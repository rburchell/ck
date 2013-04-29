QT = core dbus
TEMPLATE = lib
CONFIG += plugin no_plugin_name_prefix

target.path = /usr/lib/contextkit/subscriber-test-plugins
INSTALLS += target

SOURCES = $$PWD/timeplugin.cpp
HEADERS = $$PWD/timeplugin.h

include($$PWD/../../../common/common.pri)
INCLUDEPATH += $$PWD/../../src

