check_PROGRAMS = handlesignalrouter-unit-tests

# your test's sources
handlesignalrouter_unit_tests_SOURCES = testhandlesignalrouter.cpp	\
	propertyhandle.h testhandlesignalrouter.h

# only include these files in the coverage
COVERAGE_FILES = handlesignalrouter.cpp

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -DDEFAULT_CONTEXT_PROVIDERS=\"@DEFAULT_CONTEXT_PROVIDERS@\"
AM_LDFLAGS = 
FROM_SOURCE = handlesignalrouter.cpp handlesignalrouter.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_handlesignalrouter_unit_tests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(handlesignalrouter_unit_tests_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am
