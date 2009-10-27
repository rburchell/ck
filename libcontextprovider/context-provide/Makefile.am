bin_PROGRAMS = context-provide-internal
context_provide_internal_SOURCES = context-provide.cpp commandwatcher.cpp commandwatcher.h

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) \
	        '-DCONTEXT_LOG_MODULE_NAME="context-provide"'
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make
AM_CXXFLAGS += -I$(srcdir)/../src \
	       -I$(top_srcdir)/common

context_provide_internal_LDADD = ../src/libcontextprovider.la $(top_builddir)/common/libcommon.la

../src/libcontextprovider.la: FORCE
	$(MAKE) -C ../src libcontextprovider.la

.PHONY: FORCE

# moccing
nodist_context_provide_internal_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(context_provide_internal_SOURCES))
include $(top_srcdir)/am/qt.am
