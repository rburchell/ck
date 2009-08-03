bin_PROGRAMS = contextd
contextd_SOURCES = contextd.cpp			\
		   provider.h			\
		   loggingfeatures.h		\
		   halprovider.h		\
		   halprovider.cpp		\
		   halmanagerinterface.h	\
		   halmanagerinterface.cpp	\
		   haldeviceinterface.cpp	\
		   haldeviceinterface.h		\
		   lowmemprovider.cpp		\
		   lowmemprovider.h		\
		   boolsysfspooler.h		\
		   boolsysfspooler.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make in contextd
AM_CXXFLAGS += -I$(top_srcdir)/common				\
	       -I$(top_srcdir)/libcontextprovider/src		\
	       '-DCONTEXT_LOG_MODULE_NAME="contextd"'

contextd_LDADD = $(top_builddir)/libcontextprovider/src/libcontextprovider.la \
		 $(top_builddir)/common/libcommon.la

$(top_builddir)/libcontextprovider/src/libcontextprovider.la:
	$(MAKE) -C $(top_builddir)/libcontextprovider/src libcontextprovider.la

$(top_builddir)/common/libcommon.la:
	$(MAKE) -C $(top_builddir)/common libcommon.la

.PHONY: $(top_builddir)/libcontextprovider/src/libcontextprovider.la \
	$(top_builddir)/common/libcommon.la

# moccing
nodist_contextd_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextd_SOURCES)) 
include $(top_srcdir)/am/qt.am
