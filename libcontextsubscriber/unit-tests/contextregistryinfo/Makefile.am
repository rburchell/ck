check_PROGRAMS = contextregistryinfounittest

# your test's sources
contextregistryinfounittest_SOURCES = contextregistryinfounittest.cpp infobackend.h

COVERAGE_FILES = contextregistryinfo.cpp
EXTRA_DIST =

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
              '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"'
AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = contextregistryinfo.cpp                   \
              contextregistryinfo.h                     \
              loggingfeatures.h                         \
              contextproviderinfo.h

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_contextregistryinfounittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextregistryinfounittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

contextregistryinfounittest.o: contextregistryinfounittest.moc
