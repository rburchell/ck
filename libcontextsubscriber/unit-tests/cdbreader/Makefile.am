check_PROGRAMS = cdbreaderunittest

# test's sources
cdbreaderunittest_SOURCES = cdbreaderunittest.cpp

COVERAGE_FILES = cdbreader.cpp 
EXTRA_DIST = test.cdb

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = '-I$(srcdir)/../util/'
AM_LDFLAGS = $(CDB_LIBS)
FROM_SOURCE = cdbreader.cpp cdbreader.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_cdbreaderunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(cdbreaderunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
cdbreaderunittest.o: cdbreaderunittest.moc
