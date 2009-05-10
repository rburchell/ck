check_PROGRAMS = introspection-static

# your test's sources
introspection_static_SOURCES = contextregistryinfounittest.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = contextregistryinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp
COVERAGE_FILES = contextregistryinfo.cpp infobackend.cpp infoxmlbackend.cpp infoxmlkeysfinder.cpp

# MOC (and then compile) this headers from the source
SOURCE_H = contextregistryinfo.h infobackend.h infoxmlbackend.h infoxmlkeysfinder.h

# process this headers with MOC and compile the results
QT_TOMOC_H =

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = contextregistryinfounittest.cpp
contextregistryinfounittest.o: moc_contextregistryinfounittest_cpp.cpp

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am
