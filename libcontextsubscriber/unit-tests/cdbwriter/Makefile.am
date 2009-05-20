check_PROGRAMS = cdbwriterunittest

# your test's sources
cdbwriterunittest_SOURCES = cdbwriterunittest.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = cdbwriter.cpp
COVERAGE_FILES = cdbwriter.cpp 

# MOC (and then compile) this headers from the source
SOURCE_H = cdbwriter.h
QT_TOMOC_H =

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = cdbwriterunittest.cpp
cdbwriterunittest.o: moc_cdbwriterunittest_cpp.cpp 

LIBS += -lcdb

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am

AM_CPPFLAGS += '-I../util/'