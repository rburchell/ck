check_PROGRAMS = cdbwriterunittest

# test's sources
cdbwriterunittest_SOURCES = cdbwriterunittest.cpp

COVERAGE_FILES = cdbwriter.cpp 

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = '-I$(srcdir)/../util/'
AM_LDFLAGS = -lcdb
FROM_SOURCE = cdbwriter.cpp cdbwriter.h # copy these files from the real source
LDADD = 
include $(top_srcdir)/am/tests.am

nodist_cdbwriterunittest_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(cdbwriterunittest_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# when MOCing from .cpp file, you also has to help the dependency tracker
cdbwriterunittest.o: cdbwriterunittest.moc
