check_PROGRAMS = infoxmlbackendunittest

# your test's sources
infoxmlbackendunittest_SOURCES = infoxmlbackendunittest.cpp

COVERAGE_FILES = infoxmlbackend.cpp
EXTRA_DIST = providers1.src providers2v1.src providers2v2.src providers3.src providers4.src

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
              '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"'
AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = 							\
	infobackend.cpp infoxmlbackend.cpp			\
	cdbreader.h cdbreader.cpp				\
	infobackend.h						\
	infoxmlbackend.h 					\
	infocdbbackend.h 					\
	infocdbbackend.cpp 					\
	infokeydata.h 						\
	loggingfeatures.h                                       \
        nanoxml.cpp                                             \
        nanoxml.h                                               \
	contextproviderinfo.h

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_infoxmlbackendunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(infoxmlbackendunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

infoxmlbackendunittest.o: infoxmlbackendunittest.moc
