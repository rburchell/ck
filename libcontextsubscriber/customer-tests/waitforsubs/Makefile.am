testdir = $(libdir)/libcontextsubscriber-tests
test_PROGRAMS = waitforsubscriptiontests

testdatadir = $(datadir)/libcontextsubscriber-tests/waitforsubscription
testdata_DATA = context-provide.context

waitforsubscriptiontests_SOURCES = \
	testwaitforsubscription.cpp \
	testwaitforsubscription.h

check-customer: $(test_PROGRAMS)
	PATH=../../libcontextprovider/context-provide:$$PATH ./.libs/waitforsubscriptiontests

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtTest_CFLAGS) -I$(srcdir)/../../src \
	      -I$(top_srcdir)/common

LIBS += $(QtCore_LIBS) $(QtTest_LIBS)
waitforsubscriptiontests_LDADD = ../../src/libcontextsubscriber.la $(top_builddir)/common/libcommon.la

../../src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../../src libcontextsubscriber.la

.PHONY: FORCE

# moccing
nodist_waitforsubscriptiontests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(waitforsubscriptiontests_SOURCES))
include $(top_srcdir)/am/qt.am
