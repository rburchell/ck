check_PROGRAMS = handlesignalrouter-unit-tests

# your test's sources
handlesignalrouter_unit_tests_SOURCES = testhandlesignalrouter.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = handlesignalrouter.cpp
COVERAGE_FILES = handlesignalrouter.cpp

# MOC (and then compile) this headers from the source
SOURCE_H = handlesignalrouter.h # process this headers with MOC and compile the results
QT_TOMOC_H = testhandlesignalrouter.h propertyhandle.h

EXTRA_DIST = propertyhandle.h testhandlesignalrouter.h

# when MOCing from .cpp file, you also has to help the dependency tracker
#QT_TOMOC_CPP = testlistening.cpp
#testlistening.o: moc_testlistening_cpp.cpp

# automake can only use +=, if = was used before, so this is a must for includes
LDADD =
CLEANFILES =
check_LIBRARIES =
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am
