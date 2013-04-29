TEMPLATE = app
QT = core dbus testlib
CONFIG += link_pkgconfig
PKGCONFIG += dbus-1

isEmpty(NO_COMMON_LIB): include(../common/common.pri)

INCLUDEPATH += $$PWD/src

LIBS += -L$$PWD/src -lcontextprovider

target.path = /usr/lib/libcontextprovider-tests
INSTALLS += target
