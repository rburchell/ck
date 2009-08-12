check_PROGRAMS = serviceunittest

# test's sources
serviceunittest_SOURCES = serviceunittest.cpp manager.h manageradaptor.h property.h

COVERAGE_FILES = service.cpp 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
AM_LDFLAGS = $(QtCore_LIBS) $(QtDBus_LIBS)
FROM_SOURCE = loggingfeatures.h service.cpp service.h queuedinvoker.h queuedinvoker.cpp
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_serviceunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(serviceunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
serviceunittest.o: serviceunittest.moc
