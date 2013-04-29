include(../../test.pri)
QT += xml
TARGET = infoxmlbackendunittest

SOURCES = infoxmlbackendunittest.cpp
HEADERS = contexttyperegistryinfo.h

CONFIG += link_pkgconfig
PKGCONFIG += libcdb

INCLUDEPATH += ../util
