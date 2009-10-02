plugindir = $(libdir)/contextkit/subscriber-test-plugins/
plugin_LTLIBRARIES = contextsubscribertime2.la
contextsubscribertime2_la_SOURCES = timeplugin.cpp timeplugin.h

BUILT_SOURCES = $(contextsubscribertime2_la_SOURCES)
timeplugin.cpp: ../timeplugin.cpp
	ln -sf $< $@
timeplugin.h: ../timeplugin.h
	ln -sf $< $@

clean-local:
	rm -f timeplugin.cpp timeplugin.h

AM_CXXFLAGS = -I$(top_srcdir)/common	\
	      -I$(srcdir)/../../../src $(QtCore_CFLAGS)		\
	      $(QtDBus_CFLAGS)					\
	      '-DCONTEXT_LOG_MODULE_NAME="time2plugin"'		\
	      '-DTIME_PLUGIN_PREFIX="Time2: "'

contextsubscribertime2_la_LDFLAGS = -avoid-version -module

$(top_builddir)/common/libcommon.a:
	$(MAKE) -C $(top_builddir)/common libcommon.a

LIBS += $(CDB_LIBS) $(QtCore_LIBS) $(QtDBus_LIBS)
contextsubscribertime2_la_LIBADD=$(top_builddir)/common/libcommon.la

.PHONY: $(top_builddir)/common/libcommon.la

# moccing
nodist_contextsubscribertime2_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextsubscribertime2_la_SOURCES))
include $(top_srcdir)/am/qt.am
