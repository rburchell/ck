testdir = $(bindir)/libcontextprovider-tests
test_PROGRAMS = typestests

typestests_SOURCES = typestests.cpp typestests.h

check-customer: $(test_PROGRAMS)
	PATH=../client:$$PATH CONTEXT_LOG_VERBOSITY=WARNING ./.libs/typestests

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)  $(QtTest_CFLAGS) -I$(srcdir)/../../src -I$(top_srcdir)/common
LIBS += $(QtCore_LIBS) $(QtDBus_LIBS) $(QtTest_LIBS)
typestests_LDADD = ../../src/libcontextprovider.la
../../src/libcontextprovider.la:
	$(MAKE) -C ../../src libcontextprovider.la
.PHONY: ../../libcontextprovider.la

# moccing
nodist_typestests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(typestests_SOURCES))
include $(top_srcdir)/am/qt.am
