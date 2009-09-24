bin_PROGRAMS = new-context-provide
new_context_provide_SOURCES = context-provide.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) \
	        '-DCONTEXT_LOG_MODULE_NAME="context-provide"'
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make
AM_CXXFLAGS += -I$(srcdir)/../src \
	       -I$(top_srcdir)/common

new_context_provide_LDADD = ../src/libcontextprovider.la $(top_builddir)/common/libcommon.la

../src/libcontextprovider.la:
	$(MAKE) -C ../src libcontextprovider.la

$(top_builddir)/common/libcommon.la:
	$(MAKE) -C $(top_builddir)/common libcommon.la

.PHONY: ../src/libcontextprovider.la $(top_builddir)/common/libcommon.la

# moccing
#nodist_new_context_provide_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(new_context_provide_SOURCES))
#include $(top_srcdir)/am/qt.am
