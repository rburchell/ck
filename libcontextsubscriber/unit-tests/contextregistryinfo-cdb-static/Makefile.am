check_PROGRAMS = contextregistryinfounittest-cdb-static

# your test's sources
contextregistryinfounittest_cdb_static_SOURCES = contextregistryinfounittest.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = contextregistryinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp
COVERAGE_FILES = contextregistryinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp

# MOC (and then compile) this headers from the source
SOURCE_H = contextregistryinfo.h infobackend.h infoxmlbackend.h infocdbbackend.h infoxmlkeysfinder.h cdbreader.h # process this headers with MOC and compile the results
QT_TOMOC_H = 

EXTRA_DIST = context-providers.cdb

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = contextregistryinfounittest.cpp
contextregistryinfounittest.o: moc_contextregistryinfounittest_cpp.cpp 

LIBS += -lcdb

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am

AM_CXXFLAGS += '-DDEFAULT_REGISTRY_PATH="@DEFAULT_REGISTRY_PATH@"'
