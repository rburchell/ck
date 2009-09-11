lib_LTLIBRARIES = libcontextsubscribertime1.la 
libcontextsubscribertime1_la_SOURCES = timeplugin.cpp timeplugin.h 

BUILT_SOURCES = $(libcontextsubscribertime1_la_SOURCES)
timeplugin.cpp: ../timeplugin.cpp
	ln -sf $< $@
timeplugin.h: ../timeplugin.h
	ln -sf $< $@

clean-local:
	rm -f timeplugin.cpp timeplugin.h

AM_CXXFLAGS = -I$(top_srcdir)/common	\
	      -I$(srcdir)/../../../src $(QtCore_CFLAGS)		\
	      $(QtDBus_CFLAGS)					\
	      '-DCONTEXT_LOG_MODULE_NAME="time1plugin"'		\
	      '-DTIME_PLUGIN_PREFIX="Time1: "'

$(top_builddir)/common/libcommon.a:
	$(MAKE) -C $(top_builddir)/common libcommon.a

LIBS += $(CDB_LIBS) $(QtCore_LIBS) $(QtDBus_LIBS) 
libcontextsubscribertime1_la_LIBADD=$(top_builddir)/common/libcommon.la

.PHONY: $(top_builddir)/common/libcommon.la

# moccing
nodist_libcontextsubscribertime1_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextsubscribertime1_la_SOURCES))
include $(top_srcdir)/am/qt.am
