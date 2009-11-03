bin_PROGRAMS = context-provide-internal
context_provide_internal_SOURCES = context-provide.cpp		\
	commandwatcher.cpp commandwatcher.h propertyproxy.h	\
	propertyproxy.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) $(QJson_CFLAGS) \
	        '-DCONTEXT_LOG_MODULE_NAME="context-provide"'
LIBS += $(QtCore_LIBS) $(QJson_LIBS)

# library dependency hack for seamless make
AM_CXXFLAGS += -I$(srcdir)/../src \
	       -I$(top_srcdir)/common \
	       -I$(top_srcdir)/libcontextsubscriber/src

context_provide_internal_LDADD = ../src/libcontextprovider.la $(top_builddir)/common/libcommon.la $(top_builddir)/libcontextsubscriber/src/libcontextsubscriber.la

../src/libcontextprovider.la: FORCE
	$(MAKE) -C ../src libcontextprovider.la

../../libcontextsubscriber/src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../../libcontextsubscriber/src libcontextsubscriber.la

.PHONY: FORCE

# moccing
nodist_context_provide_internal_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(context_provide_internal_SOURCES))
include $(top_srcdir)/am/qt.am
