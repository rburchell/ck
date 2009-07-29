bin_PROGRAMS = contextd
contextd_SOURCES = contextd.cpp		        \
		   provider.h		        \
		   logging.cpp		        \
		   logging.h		        \
		   loggingfeatures.h	        \
		   halprovider.h	        \
		   halprovider.cpp	        \
		   halmanagerinterface.h	\
		   halmanagerinterface.cpp      \
		   haldeviceinterface.cpp	\
		   haldeviceinterface.h         \
		   sconnect.h			\
		   lowmemprovider.cpp		\
		   lowmemprovider.h		\
		   boolsysfspooler.h		\
		   boolsysfspooler.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make in contextd
AM_CXXFLAGS += -I$(srcdir)/../../libcontextprovider/src		\
	      '-DCONTEXT_LOG_MODULE_NAME="contextd"'

contextd_LDADD = ../../libcontextprovider/src/libcontextprovider.la
../../libcontextprovider/src/libcontextprovider.la:
	$(MAKE) -C ../../libcontextprovider/src libcontextprovider.la
.PHONY: ../../libcontextprovider/src/libcontextprovider.la

# moccing
nodist_contextd_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(contextd_SOURCES))
include $(top_srcdir)/am/qt.am
