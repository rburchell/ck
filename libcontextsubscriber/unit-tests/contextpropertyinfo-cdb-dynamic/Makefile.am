check_PROGRAMS = contextpropertyinfounittest-cdb-dynamic

# your test's sources
contextpropertyinfounittest_cdb_dynamic_SOURCES = contextpropertyinfounittest.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = contextregistryinfo.cpp contextpropertyinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp
COVERAGE_FILES = contextregistryinfo.cpp contextpropertyinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp

# MOC (and then compile) this headers from the source
SOURCE_H = contextregistryinfo.h contextpropertyinfo.h infobackend.h infoxmlbackend.h infocdbbackend.h infoxmlkeysfinder.h cdbreader.h # process this headers with MOC and compile the results
QT_TOMOC_H = 

EXTRA_DIST = context-providers2v1.cdb context-providers2v2.cdb context-providers3v1.cdb context-providers3v2.cdb context-providers4v1.cdb context-providers4v2.cdb

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = contextpropertyinfounittest.cpp
contextpropertyinfounittest.o: moc_contextpropertyinfounittest_cpp.cpp 

LIBS += -lcdb

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am

AM_CPPFLAGS += '-DDEFAULT_REGISTRY_PATH="@DEFAULT_REGISTRY_PATH@"'
AM_CPPFLAGS += '-I../util/'
