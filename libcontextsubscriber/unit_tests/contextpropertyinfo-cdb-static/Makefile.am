check_PROGRAMS = contextpropertyinfounittest

# your test's sources
contextpropertyinfounittest_SOURCES = contextpropertyinfounittest.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = contextregistryinfo.cpp contextpropertyinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp
COVERAGE_FILES = contextregistryinfo.cpp contextpropertyinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp infocdbbackend.cpp cdbreader.cpp

# MOC (and then compile) this headers from the source
SOURCE_H = contextregistryinfo.h contextpropertyinfo.h infobackend.h infoxmlbackend.h infocdbbackend.h infoxmlkeysfinder.h cdbreader.h # process this headers with MOC and compile the results
QT_TOMOC_H = 

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
