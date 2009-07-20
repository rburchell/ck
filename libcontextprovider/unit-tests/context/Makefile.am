check_PROGRAMS = contextunittest

# test's sources
contextunittest_SOURCES = contextunittest.cpp manager.h manageradaptor.h

COVERAGE_FILES = context.cpp 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
AM_LDFLAGS = $(QtCore_LIBS) $(QtDBus_LIBS)
FROM_SOURCE = queuedinvoker.cpp queuedinvoker.h subscriber.h subscriber.cpp loggingfeatures.h logging.cpp logging.h context.cpp context.h sconnect.h subscriberadaptor.cpp subscriberadaptor.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_contextunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
contextunittest.o: contextunittest.moc
