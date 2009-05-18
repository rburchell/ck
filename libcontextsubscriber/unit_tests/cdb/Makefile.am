check_PROGRAMS = cdb

# your test's sources
cdb_SOURCES = cdbwriterunittest.cpp cdbreaderunittest.cpp cdb.cpp

# sources tested, all of the will be recompiled specificly for the tests
SOURCE_CXX = cdbwriter.cpp cdbreader.cpp
COVERAGE_FILES =  cdbwriter.cpp cdbreader.cpp 

# MOC (and then compile) this headers from the source
SOURCE_H = cdbwriter.h cdbreader.h # process this headers with MOC and compile the results
QT_TOMOC_H = cdbreaderunittest.h cdbwriterunittest.h 

# when MOCing from .cpp file, you also has to help the dependency tracker
QT_TOMOC_CPP = cdbwriterunittest.cpp cdbreaderunittest.cpp
cdbwriterunittest.o: moc_cdbwriterunittest_cpp.cpp 
cdbreaderunittest.o: moc_cdbreaderunittest_cpp.cpp 

LIBS += -lcdb

# automake can only use +=, if = was used before, so this is a must for includes
LDADD = 
CLEANFILES =
check_LIBRARIES = 
include $(top_srcdir)/am/tests.am
include $(top_srcdir)/am/qt-check.am
