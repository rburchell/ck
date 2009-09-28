check_PROGRAMS = nanoxmlunittest

# test's sources
nanoxmlunittest_SOURCES = nanoxmlunittest.cpp

COVERAGE_FILES = nanoxml.cpp 
EXTRA_DIST = test1.xml broken.xml test2.xml test3.xml test4.xml

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtXml_CFLAGS)
AM_LDFLAGS = $(QtXml_LIBS)
FROM_SOURCE = nanoxml.cpp nanoxml.h loggingfeatures.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_nanoxmlunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(nanoxmlunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
nanoxmlunittest.o: nanoxmlunittest.moc
