testdir = $(libdir)/libcontextsubscriber-tests
test_PROGRAMS = regressiontests

testdatadir = $(datadir)/libcontextsubscriber-tests/regression
testdata_DATA = context-provide.context

regressiontests_SOURCES = \
	testregression.cpp \
	testregression.h

check-customer: $(test_PROGRAMS)
	./.libs/regressiontests

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtTest_CFLAGS) -I$(srcdir)/../../src \
	      -I$(top_srcdir)/common

LIBS += $(QtCore_LIBS) $(QtTest_LIBS)
regressiontests_LDADD = ../../src/libcontextsubscriber.la $(top_builddir)/common/libcommon.la

../../src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../../src libcontextsubscriber.la

.PHONY: FORCE

# moccing
nodist_regressiontests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(regressiontests_SOURCES))
include $(top_srcdir)/am/qt.am
