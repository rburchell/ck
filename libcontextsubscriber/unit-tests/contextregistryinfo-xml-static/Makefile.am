check_PROGRAMS = contextregistryinfounittest-xml-static

# test's sources
contextregistryinfounittest_xml_static_SOURCES = contextregistryinfounittest.cpp

COVERAGE_FILES = contextregistryinfo.cpp 	\
		 infobackend.cpp 		\
		 infoxmlbackend.cpp 		\
		 infoxmlkeysfinder.cpp

EXTRA_DIST = providers1.context providers2.context

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = '-I$(srcdir)/../util/' '-DDEFAULT_CONTEXT_PROVIDERS="@DEFAULT_CONTEXT_PROVIDERS@"' $(QtXml_CFLAGS)
AM_LDFLAGS = -lcdb $(QtXml_LIBS)
FROM_SOURCE = contextregistryinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp contextregistryinfo.h infobackend.h infoxmlbackend.h infocdbbackend.h infoxmlkeysfinder.h cdbreader.h infokeydata.h sconnect.h # copy these files from the real source
LDADD =
include $(top_srcdir)/am/tests.am

nodist_contextregistryinfounittest_xml_static_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextregistryinfounittest_xml_static_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also have to help the dependency tracker
contextregistryinfounittest.o: contextregistryinfounittest.moc

