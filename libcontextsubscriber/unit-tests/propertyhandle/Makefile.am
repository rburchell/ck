check_PROGRAMS = propertyhandle-unit-tests

# your test's sources
propertyhandle_unit_tests_SOURCES = testpropertyhandle.cpp		\
	testpropertyhandle.h propertyprovider.h dbusnamelistener.h	\
	contextpropertyinfo.h contextregistryinfo.h queuedinvoker.h

# only include these files in the coverage
COVERAGE_FILES = propertyhandle.cpp

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -DDEFAULT_CONTEXT_PROVIDERS=\"@DEFAULT_CONTEXT_PROVIDERS@\"
AM_LDFLAGS = 
FROM_SOURCE = propertyhandle.cpp propertyhandle.h sconnect.h # copy these files from the real source
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_propertyhandle_unit_tests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(propertyhandle_unit_tests_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am
