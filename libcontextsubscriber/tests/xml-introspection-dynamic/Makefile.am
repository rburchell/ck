check_PROGRAMS = xml-introspection-dynamic

# your test's sources
xml_introspection_dynamic_SOURCES = contextregistryinfounittest.cpp contextpropertyinfounittest.cpp xml-introspection-dynamic.cpp 

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = contextregistryinfo.cpp contextpropertyinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp
COVERAGE_FILES = contextregistryinfo.cpp contextpropertyinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp

# MOC (and then compile) this headers from the source
SOURCE_H = contextregistryinfo.h contextpropertyinfo.h infobackend.h infoxmlbackend.h infoxmlkeysfinder.h # process this headers with MOC and compile the results
QT_TOMOC_H = contextregistryinfounittest.h contextpropertyinfounittest.h

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = contextregistryinfounittest.cpp contextpropertyinfounittest.cpp
contextregistryinfounittest.o: moc_contextregistryinfounittest_cpp.cpp 
contextpropertyinfounittest.o: moc_contextpropertyinfounittest_cpp.cpp 

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am
