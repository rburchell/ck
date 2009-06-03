check_PROGRAMS = contextpropertyinfounittest-xml-static

# your test's sources
contextpropertyinfounittest_xml_static_SOURCES = contextpropertyinfounittest.cpp

COVERAGE_FILES = contextregistryinfo.cpp contextpropertyinfo.cpp	\
	infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp	\
	infocdbbackend.cpp cdbreader.cpp
EXTRA_DIST = providers1.context providers2.context

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ -DDEFAULT_CONTEXT_PROVIDERS="\"@DEFAULT_CONTEXT_PROVIDERS@\""
AM_LDFLAGS = -lcdb $(QtXml_LIBS)
FROM_SOURCE = src_contextregistryinfo.cpp src_contextpropertyinfo.cpp	\
	src_infobackend.cpp src_infoxmlbackend.cpp			\
	src_infoxmlkeysfinder.cpp src_infocdbbackend.cpp		\
	src_cdbreader.cpp src_contextregistryinfo.h			\
	src_contextpropertyinfo.h src_infobackend.h			\
	src_infoxmlbackend.h src_infocdbbackend.h			\
	src_infoxmlkeysfinder.h src_cdbreader.h				\
	src_logging.cpp src_logging.h
LDADD =
include $(top_srcdir)/am/tests.am

nodist_contextpropertyinfounittest_xml_static_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextpropertyinfounittest_xml_static_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

contextpropertyinfounittest.o: contextpropertyinfounittest.moc
