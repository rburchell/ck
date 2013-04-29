include(../../test.pri)
TARGET = servicetest

SOURCES = servicetest.cpp
HEADERS = servicetest.h

testdir = $(libdir)/libcontextprovider-tests
test_PROGRAMS = servicetest

