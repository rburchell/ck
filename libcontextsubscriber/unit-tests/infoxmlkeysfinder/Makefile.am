check_PROGRAMS = infoxmlkeysfinder-unit-tests

# your test's sources
infoxmlkeysfinder_unit_tests_SOURCES = testinfoxmlkeysfinder.cpp	\
                                       testinfoxmlkeysfinder.h

# only include these files in the coverage
COVERAGE_FILES = infoxmlkeysfinder.cpp
EXTRA_DIST = provider-alltypes.context provider-complextypes.context	\
             provider-dbus.context provider-plugin.context

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtXml_CFLAGS) '-I$(srcdir)/../util/'
AM_LDFLAGS = $(QtXml_LIBS)
FROM_SOURCE = infoxmlkeysfinder.cpp infoxmlkeysfinder.h infokeydata.h loggingfeatures.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_infoxmlkeysfinder_unit_tests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(infoxmlkeysfinder_unit_tests_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am
