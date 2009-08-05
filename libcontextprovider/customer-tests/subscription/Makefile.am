bin_PROGRAMS = subscriptiontests

subscriptiontests_SOURCES = subscriptiontests.cpp sconnect.h \
							subscriptiontests.h

check-customer:
	PATH=$$PATH:../client CONTEXT_LOG_VERBOSITY=WARNING ./subscriptiontests

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)  $(QtTest_CFLAGS) -I$(srcdir)/../../src
LIBS += $(QtCore_LIBS) $(QtDBus_LIBS) $(QtTest_LIBS)
subscriptiontests_LDADD = ../../src/libcontextprovider.la
../../src/libcontextprovider.la:
	$(MAKE) -C ../../src libcontextprovider.la
.PHONY: ../../libcontextprovider.la

# moccing
nodist_subscriptiontests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(subscriptiontests_SOURCES))
include $(top_srcdir)/am/qt.am
