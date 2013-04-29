TEMPLATE = app
QT = core dbus testlib
CONFIG += link_pkgconfig
PKGCONFIG += dbus-1

isEmpty(NO_COMMON_LIB): include(../common/common.pri)

INCLUDEPATH += $$PWD/src

LIBS += -L$$PWD/src -lcontextsubscriber

target.path = /usr/lib/libcontextsubscriber-tests
INSTALLS += target
