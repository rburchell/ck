check_PROGRAMS = contextpropertyinfounittest-xml-static

# your test's sources
contextpropertyinfounittest_xml_static_SOURCES = contextpropertyinfounittest.cpp

COVERAGE_FILES = contextpropertyinfo.cpp	\
		 infobackend.cpp 		\
		 infoxmlbackend.cpp
EXTRA_DIST = providers1.context providers2.context

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ \
              '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
              '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"'
AM_LDFLAGS = $(CDB_LIBS) $(QtXml_LIBS)
FROM_SOURCE = contextregistryinfo.cpp contextpropertyinfo.cpp	\
	infobackend.cpp infoxmlbackend.cpp			\
	infocdbbackend.cpp		                        \
	cdbreader.cpp contextregistryinfo.h			\
	contextpropertyinfo.h infobackend.h			\
	infoxmlbackend.h infocdbbackend.h			\
	cdbreader.h				                \
	infokeydata.h                                           \
	loggingfeatures.h nanoxml.h nanoxml.cpp

FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_contextpropertyinfounittest_xml_static_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextpropertyinfounittest_xml_static_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

contextpropertyinfounittest.o: contextpropertyinfounittest.moc
