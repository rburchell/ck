bin_PROGRAMS = context-ls
context_ls_SOURCES = context-ls.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) \
	      '-DCONTEXT_LOG_MODULE_NAME="context-ls"'
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make in cls/
AM_CXXFLAGS += -I$(srcdir)/../src \
	       -I$(top_srcdir)/common

context_ls_LDADD = ../src/libcontextsubscriber.la $(top_builddir)/common/libcommon.la

../src/libcontextsubscriber.la:
	$(MAKE) -C ../src libcontextsubscriber.la

$(top_builddir)/common/libcommon.la:
	$(MAKE) -C $(top_builddir)/common libcommon.la

.PHONY: ../src/libcontextsubscriber.la $(top_builddir)/common/libcommon.la

# moccing
# nodist_context_ls_SOURCES = mocs.cpp
# QT_TOMOC = $(filter %.h, $(context_ls_SOURCES))
# include $(top_srcdir)/am/qt.am
