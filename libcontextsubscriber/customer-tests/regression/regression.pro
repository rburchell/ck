include(../../test.pri)
TARGET = regressiontests

testdata.path = /usr/share/libcontextsubscriber-tests/regression
testdata.files = context-provide.context
INSTALLS += testdata

SOURCES = testregression.cpp
HEADERS = testregression.h
