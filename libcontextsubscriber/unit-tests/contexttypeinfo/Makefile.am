check_PROGRAMS = contexttypeinfounittest

# test's sources
contexttypeinfounittest_SOURCES = contexttypeinfounittest.cpp

COVERAGE_FILES = contexttypeinfo.cpp
EXTRA_DIST = types.xml

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtXml_CFLAGS) -I$(srcdir)/../util/
AM_CPPFLAGS = -DDEFAULT_CONTEXT_TYPES="\"BAD\"" \
	-DDEFAULT_CONTEXT_CORE_TYPES="\"WORSE\""
AM_LDFLAGS = $(QtXml_LIBS)
FROM_SOURCE = contexttypeinfo.cpp contexttypeinfo.h \
	assoctree.cpp assoctree.h loggingfeatures.h \
	contexttyperegistryinfo.cpp contexttyperegistryinfo.h \
	nanoxml.cpp nanoxml.h
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_contexttypeinfounittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contexttypeinfounittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
contexttypeinfounittest.o: contexttypeinfounittest.moc
