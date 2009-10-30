check_PROGRAMS = contexttypeinfounittest

# test's sources
contexttypeinfounittest_SOURCES = contexttypeinfounittest.cpp

COVERAGE_FILES = contexttypeinfo.cpp 
EXTRA_DIST = double.xml custom-double.xml uniform-list.xml person.xml

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtXml_CFLAGS) -I$(srcdir)/../util/
AM_LDFLAGS = $(QtXml_LIBS)
FROM_SOURCE = contexttypeinfo.cpp contexttypeinfo.h nanotree.cpp nanotree.h nanoxml.cpp nanoxml.h loggingfeatures.h # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_contexttypeinfounittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contexttypeinfounittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
contexttypeinfounittest.o: contexttypeinfounittest.moc
