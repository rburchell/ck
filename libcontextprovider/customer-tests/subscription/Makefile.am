bin_PROGRAMS = subscriptiontests

subscriptiontests_SOURCES = subscriptiontests.cpp \
			    subscriptiontests.h

check-customer:
	export PATH=$$PATH:../client ; ./subscriptiontests

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)  $(QtTest_CFLAGS) -I$(srcdir)/../../src \
	      -I$(top_srcdir)/common

LIBS += $(QtCore_LIBS) $(QtDBus_LIBS) $(QtTest_LIBS)
subscriptiontests_LDADD = ../../src/libcontextprovider.la $(top_builddir)/common/libcommon.la

../../src/libcontextprovider.la:
	$(MAKE) -C ../../src libcontextprovider.la

$(top_builddir)/common/libcommon.la:
	$(MAKE) -C $(top_builddir)/common libcommon.la
	
.PHONY: ../../libcontextprovider.la $(top_builddir)/libcommon.la

# moccing
nodist_subscriptiontests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(subscriptiontests_SOURCES))
include $(top_srcdir)/am/qt.am
