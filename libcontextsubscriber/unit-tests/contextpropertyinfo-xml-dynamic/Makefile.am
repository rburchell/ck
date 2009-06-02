check_PROGRAMS = contextpropertyinfounittest-xml-dynamic

# your test's sources
contextpropertyinfounittest_xml_dynamic_SOURCES = contextpropertyinfounittest.cpp

COVERAGE_FILES = contextregistryinfo.cpp contextpropertyinfo.cpp	\
	infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp	\
	infocdbbackend.cpp cdbreader.cpp
EXTRA_DIST = providers2v1.xml.src providers2v2.xml.src			\
	providers3v1.xml.src providers3v2.xml.src providers4v1.xml.src	\
	providers4v2.xml.src

# do the testing, coverage, etc. stuff
# tests-new.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) -I$(srcdir)/../util/ -DDEFAULT_CONTEXT_PROVIDERS="\"@DEFAULT_CONTEXT_PROVIDERS@\""
AM_LDFLAGS = -lcdb $(QtXml_LIBS)
FROM_SOURCE = src_contextregistryinfo.cpp src_contextpropertyinfo.cpp	\
	src_infobackend.cpp src_infoxmlbackend.cpp			\
	src_infoxmlkeysfinder.cpp src_infocdbbackend.cpp		\
	src_cdbreader.cpp src_contextregistryinfo.h			\
	src_contextpropertyinfo.h src_infobackend.h			\
	src_infoxmlbackend.h src_infocdbbackend.h			\
	src_infoxmlkeysfinder.h src_cdbreader.h
LDADD =
include $(top_srcdir)/am/tests-new.am

nodist_contextpropertyinfounittest_xml_dynamic_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextpropertyinfounittest_xml_dynamic_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt-new.am

contextpropertyinfounittest.o: contextpropertyinfounittest.moc
