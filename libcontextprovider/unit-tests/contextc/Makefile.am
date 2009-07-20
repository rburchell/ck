check_PROGRAMS = contextcunittest

# test's sources
contextcunittest_SOURCES = contextcunittest.cpp context.h

COVERAGE_FILES = contextc.cpp 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
AM_LDFLAGS = $(QtCore_LIBS) $(QtDBus_LIBS)
FROM_SOURCE = logging.cpp logging.h loggingfeatures.h contextc.h contextc.cpp
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_contextcunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextcunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
contextcunittest.o: contextcunittest.moc
