check_PROGRAMS = cdbunittest

# test's sources
cdbunittest_SOURCES = cdbunittest.cpp

COVERAGE_FILES = cdbwriter.cpp cdbreader.cpp

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS) '-I$(srcdir)/../util/'
AM_LDFLAGS = $(QtCore_LIBS) $(CDB_LIBS)
FROM_SOURCE = cdbreader.h cdbreader.cpp cdbwriter.cpp cdbwriter.h loggingfeatures.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_cdbunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(cdbunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
cdbunittest.o: cdbunittest.moc
