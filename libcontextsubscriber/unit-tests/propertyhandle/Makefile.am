check_PROGRAMS = propertyhandle-unit-tests

# your test's sources
propertyhandle_unit_tests_SOURCES = testpropertyhandle.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = propertyhandle.cpp
COVERAGE_FILES = propertyhandle.cpp

# MOC (and then compile) this headers from the source
SOURCE_H = propertyhandle.h # process this headers with MOC and compile the results
QT_TOMOC_H = testpropertyhandle.h propertyprovider.h dbusnamelistener.h contextpropertyinfo.h contextregistryinfo.h

EXTRA_DIST = contextpropertyinfo.h contextregistryinfo.h dbusnamelistener.h propertyprovider.h sconnect.h testpropertyhandle.h

# when MOCing from .cpp file, you also has to help the dependency tracker
#QT_TOMOC_CPP = testlistening.cpp
#testlistening.o: moc_testlistening_cpp.cpp

# automake can only use +=, if = was used before, so this is a must for includes
LDADD =
CLEANFILES =
check_LIBRARIES =
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am
