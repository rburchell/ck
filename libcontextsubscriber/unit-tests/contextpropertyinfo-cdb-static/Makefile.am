check_PROGRAMS = contextpropertyinfounittest-cdb-static

# your test's sources
contextpropertyinfounittest_cdb_static_SOURCES = contextpropertyinfounittest.cpp

COVERAGE_FILES = contextpropertyinfo.cpp 	\
		 infobackend.cpp 		\
		 infocdbbackend.cpp
EXTRA_DIST = cache.cdb

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -DDEFAULT_CONTEXT_PROVIDERS="\"@DEFAULT_CONTEXT_PROVIDERS@\"" -I$(srcdir)/../util/
AM_LDFLAGS = -lcdb $(QtXml_LIBS)
FROM_SOURCE = contextregistryinfo.cpp contextpropertyinfo.cpp	\
	infobackend.cpp infoxmlbackend.cpp			\
	infoxmlkeysfinder.cpp infocdbbackend.cpp		\
	cdbreader.cpp contextregistryinfo.h			\
	contextpropertyinfo.h infobackend.h			\
	infoxmlbackend.h infocdbbackend.h			\
	infoxmlkeysfinder.h cdbreader.h                         \
	infokeydata.h sconnect.h logging.h logging.cpp
LDADD =
include $(top_srcdir)/am/tests.am

nodist_contextpropertyinfounittest_cdb_static_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextpropertyinfounittest_cdb_static_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

contextpropertyinfounittest.o: contextpropertyinfounittest.moc
