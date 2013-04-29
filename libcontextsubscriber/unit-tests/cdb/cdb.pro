include(../../test.pri)
TARGET = cdbunittest

SOURCES = cdbunittest.cpp

CONFIG += link_pkgconfig
PKGCONFIG += libcdb

INCLUDEPATH += ../util
