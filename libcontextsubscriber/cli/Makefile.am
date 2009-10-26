bin_PROGRAMS = context-listen
context_listen_SOURCES = context-listen.cpp			        \
	propertylistener.h propertylistener.cpp commandwatcher.h	\
	commandwatcher.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) \
	      '-DCONTEXT_LOG_MODULE_NAME="context-listen"'
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make in cli/
AM_CXXFLAGS += -I$(srcdir)/../src \
	       -I$(top_srcdir)/common

context_listen_LDADD = ../src/libcontextsubscriber.la $(top_builddir)/common/libcommon.la

../src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../src libcontextsubscriber.la

.PHONY: FORCE

# moccing
nodist_context_listen_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(context_listen_SOURCES))
include $(top_srcdir)/am/qt.am
