testdir = $(libdir)/libcontextprovider-tests
test_PROGRAMS = valuechangestests

valuechangestests_SOURCES = valuechangestests.cpp valuechangestests.h

check-customer:
	PATH=$$PATH:../client CONTEXT_LOG_VERBOSITY=WARNING ./valuechangestests

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)  $(QtTest_CFLAGS) -I$(srcdir)/../../src -I$(top_srcdir)/common
LIBS += $(QtCore_LIBS) $(QtDBus_LIBS) $(QtTest_LIBS)
valuechangestests_LDADD = ../../src/libcontextprovider.la
../../src/libcontextprovider.la:
	$(MAKE) -C ../../src libcontextprovider.la
.PHONY: ../../libcontextprovider.la

# moccing
nodist_valuechangestests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(valuechangestests_SOURCES))
include $(top_srcdir)/am/qt.am
