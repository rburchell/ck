check_PROGRAMS = managerunittest

# test's sources
managerunittest_SOURCES = managerunittest.cpp

COVERAGE_FILES = manager.cpp 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
AM_LDFLAGS = $(QtCore_LIBS) $(QtDBus_LIBS)
FROM_SOURCE = queuedinvoker.cpp queuedinvoker.h sconnect.h subscriber.h subscriber.cpp logging.cpp logging.h loggingfeatures.h manager.cpp manager.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_managerunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(managerunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
managerunittest.o: managerunittest.moc
