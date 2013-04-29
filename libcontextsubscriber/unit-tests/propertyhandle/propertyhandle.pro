include(../../test.pri)
TARGET = propertyhandle-unit-tests

SOURCES = testpropertyhandle.cpp
HEADERS = testpropertyhandle.h provider.h dbusnamelistener.h \
          contextpropertyinfo.h contextregistryinfo.h \
          contexttypeinfo.h

LIBS += -lrt
