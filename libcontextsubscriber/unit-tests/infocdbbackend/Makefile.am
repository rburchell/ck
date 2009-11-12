check_PROGRAMS = infocdbbackendunittest

# your test's sources
infocdbbackendunittest_SOURCES = infocdbbackendunittest.cpp contexttyperegistryinfo.h

COVERAGE_FILES = infocdbbackend.cpp
EXTRA_DIST = 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
              '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"' \
              '-DDEFAULT_CONTEXT_TYPES="@datadir@/contextkit/types/"' \
              '-DDEFAULT_CONTEXT_CORE_TYPES="@datadir@/contextkit/core.types"'

AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = 							\
	infobackend.cpp infoxmlbackend.cpp			\
	cdbreader.h cdbreader.cpp				\
        cdbwriter.h cdbwriter.cpp                               \
	infobackend.h						\
	infoxmlbackend.h 					\
	infocdbbackend.h 					\
	infocdbbackend.cpp 					\
	infokeydata.h 						\
	loggingfeatures.h                                       \
        nanoxml.h                                               \
        nanoxml.cpp						\
	contextproviderinfo.h					\
	assoctree.h						\
	assoctree.cpp						\
	contexttypeinfo.h					\
	contexttypeinfo.cpp

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_infocdbbackendunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(infocdbbackendunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

infocdbbackendunittest.o: infocdbbackendunittest.moc
