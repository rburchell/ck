check_PROGRAMS = contexttyperegistryinfounittest

# your test's sources
contexttyperegistryinfounittest_SOURCES = contexttyperegistryinfounittest.cpp

COVERAGE_FILES = contexttyperegistryinfo.cpp
EXTRA_DIST =

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
              '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"' \
              '-DDEFAULT_CONTEXT_TYPES="@datadir@/contextkit/types/"' \
              '-DDEFAULT_CONTEXT_CORE_TYPES="@datadir@/contextkit/core.types"'
AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = contexttyperegistryinfo.cpp			\
              contexttyperegistryinfo.h				\
              loggingfeatures.h					\
              nanotree.h					\
              nanotree.cpp					\
	      nanoxml.h						\
	      nanoxml.cpp

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_contexttyperegistryinfounittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contexttyperegistryinfounittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

contexttyperegistryinfounittest.o: contexttyperegistryinfounittest.moc
