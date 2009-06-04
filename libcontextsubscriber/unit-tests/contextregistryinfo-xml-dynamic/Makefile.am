check_PROGRAMS = contextregistryinfounittest-xml-dynamic

# test's sources
contextregistryinfounittest_xml_dynamic_SOURCES = contextregistryinfounittest.cpp

COVERAGE_FILES = contextregistryinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp

EXTRA_DIST = providers1v1.xml.src providers1v2.xml.src

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = '-I$(srcdir)/../util/' '-DDEFAULT_CONTEXT_PROVIDERS="@DEFAULT_CONTEXT_PROVIDERS@"' $(QtXml_CFLAGS)
AM_LDFLAGS = -lcdb $(QtXml_LIBS)
FROM_SOURCE = src_contextregistryinfo.cpp src_infobackend.cpp src_infoxmlbackend.cpp src_infoxmlkeysfinder.cpp src_infocdbbackend.cpp src_cdbreader.cpp src_contextregistryinfo.h src_infobackend.h src_infoxmlbackend.h src_infocdbbackend.h src_infoxmlkeysfinder.h src_cdbreader.h # copy these files from the real source
LDADD =
include $(top_srcdir)/am/tests.am

nodist_contextregistryinfounittest_xml_dynamic_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextregistryinfounittest_xml_dynamic_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also have to help the dependency tracker
contextregistryinfounittest.o: contextregistryinfounittest.moc

