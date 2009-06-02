check_PROGRAMS = propertyprovider-unit-tests

# your test's sources
propertyprovider_unit_tests_SOURCES = testpropertyprovider.cpp		\
	testpropertyprovider.h managerinterface.h dbusnamelistener.h	\
	subscriberinterface.h handlesignalrouter.h

# only include these files in the coverage
COVERAGE_FILES = propertyprovider.cpp

# do the testing, coverage, etc. stuff
# tests-new.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS)
AM_LDFLAGS = 
FROM_SOURCE = src_propertyprovider.cpp src_propertyprovider.h # copy these files from the real source
LDADD =
include $(top_srcdir)/am/tests-new.am

nodist_propertyprovider_unit_tests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(propertyprovider_unit_tests_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt-new.am
