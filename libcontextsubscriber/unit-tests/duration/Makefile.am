check_PROGRAMS = testduration

# your test's sources
testduration_SOURCES = testduration.cpp

# only include these files in the coverage
COVERAGE_FILES = duration.cpp

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS)
AM_LDFLAGS = $(QtCore_LIBS)
FROM_SOURCE = duration.h duration.cpp
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_testduration_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(testduration_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am
testduration.o: testduration.moc
