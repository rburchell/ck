check_PROGRAMS = contextregistryinfounittest-cdb-static

# your test's sources
contextregistryinfounittest_cdb_static_SOURCES = contextregistryinfounittest.cpp

COVERAGE_FILES = contextregistryinfo.cpp 	\
		 infobackend.cpp		\
		 infocdbbackend.cpp
EXTRA_DIST = cache.cdb

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"'
AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = contextregistryinfo.cpp infobackend.cpp	\
	infoxmlbackend.cpp infoxmlkeysfinder.cpp	\
	infocdbbackend.cpp cdbreader.cpp		\
	contextregistryinfo.h infobackend.h		\
	infoxmlbackend.h infocdbbackend.h		\
	infoxmlkeysfinder.h cdbreader.h                 \
	infokeydata.h \
	loggingfeatures.h

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_contextregistryinfounittest_cdb_static_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextregistryinfounittest_cdb_static_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

contextregistryinfounittest.o: contextregistryinfounittest.moc
