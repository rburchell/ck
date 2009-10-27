check_PROGRAMS = propertyunittest

# test's sources
propertyunittest_SOURCES = propertyunittest.cpp service.h servicebackend.h property.h propertyprivate.h

COVERAGE_FILES = property.cpp propertyprivate.cpp

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
AM_LDFLAGS = $(QtCore_LIBS) $(QtDBus_LIBS)
FROM_SOURCE = loggingfeatures.h property.cpp propertyprivate.cpp # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_propertyunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(propertyunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
propertyunittest.o: propertyunittest.moc
