check_PROGRAMS = assoctreeunittest

# test's sources
assoctreeunittest_SOURCES = assoctreeunittest.cpp

COVERAGE_FILES = assoctree.cpp 
EXTRA_DIST = 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtXml_CFLAGS) -I$(srcdir)/../util/
AM_LDFLAGS = $(QtXml_LIBS)
FROM_SOURCE = assoctree.cpp assoctree.h loggingfeatures.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_assoctreeunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(assoctreeunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
assoctreeunittest.o: assoctreeunittest.moc
