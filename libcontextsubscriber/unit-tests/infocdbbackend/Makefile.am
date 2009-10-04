check_PROGRAMS = infocdbbackendunittest

# your test's sources
infocdbbackendunittest_SOURCES = infocdbbackendunittest.cpp

COVERAGE_FILES = infocdbbackend.cpp
EXTRA_DIST = 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
              '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"'
AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = 							\
	infobackend.cpp infoxmlbackend.cpp			\
	cdbreader.h cdbreader.cpp				\
        cdbwriter.h cdbwriter.cpp                               \
	infoxmlkeysfinder.cpp 					\
	infobackend.h						\
	infoxmlbackend.h 					\
	infocdbbackend.h 					\
	infocdbbackend.cpp 					\
	infoxmlkeysfinder.h 					\
	infokeydata.h 						\
	loggingfeatures.h

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_infocdbbackendunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(infocdbbackendunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

infocdbbackendunittest.o: infocdbbackendunittest.moc
