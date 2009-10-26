check_PROGRAMS = contextpropertyinfounittest

# your test's sources
contextpropertyinfounittest_SOURCES = contextpropertyinfounittest.cpp infobackend.h

COVERAGE_FILES = contextpropertyinfo.cpp
EXTRA_DIST =

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
              '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"'
AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = contextpropertyinfo.cpp                   \
              contextpropertyinfo.h                     \
              loggingfeatures.h                         \
              contextproviderinfo.h                     \
              contexttypeinfo.h				\
	      contexttypeinfo.cpp			\
	      nanotree.cpp				\
	      nanotree.h

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_contextpropertyinfounittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextpropertyinfounittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

contextpropertyinfounittest.o: contextpropertyinfounittest.moc
