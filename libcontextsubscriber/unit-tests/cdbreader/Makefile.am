check_PROGRAMS = cdbreaderunittest

# your test's sources
cdbreaderunittest_SOURCES = cdbreaderunittest.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = cdbreader.cpp
COVERAGE_FILES = cdbreader.cpp 

# MOC (and then compile) this headers from the source
SOURCE_H = cdbreader.h
QT_TOMOC_H =

EXTRA_DIST = test.cdb

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = cdbreaderunittest.cpp
cdbreaderunittest.o: moc_cdbreaderunittest_cpp.cpp 

LIBS += -lcdb

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am

AM_CPPFLAGS += '-I../util/'
