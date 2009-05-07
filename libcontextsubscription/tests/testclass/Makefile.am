check_PROGRAMS = testclassunittest

# your test's sources
testclassunittest_SOURCES = testclassunittest.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = testclass.cpp
COVERAGE_FILES = testclass.cpp 

# MOC (and then compile) this headers from the source
SOURCE_H = testclass.h

# process this headers with MOC and compile the results
QT_TOMOC_H =

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = testclassunittest.cpp
testclassunittest.o: moc_testclassunittest_cpp.cpp

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am
