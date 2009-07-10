lib_LTLIBRARIES = libcontextsubscriber.la
libcontextsubscriber_la_SOURCES = contextproperty.cpp			\
	  contextproperty.h propertyhandle.cpp propertyhandle.h		\
	  propertyprovider.cpp propertyprovider.h managerinterface.h	\
	  managerinterface.cpp safedbuspendingcallwatcher.h		\
	  subscriberinterface.h subscriberinterface.cpp			\
	  contextpropertyinfo.cpp contextpropertyinfo.h			\
	  contextregistryinfo.cpp contextregistryinfo.h infobackend.h	\
	  infobackend.cpp infoxmlbackend.h infoxmlbackend.cpp		\
	  infoxmlkeysfinder.h infoxmlkeysfinder.cpp infokeydata.h	\
	  cdbwriter.h cdbwriter.cpp cdbreader.cpp cdbreader.h		\
	  infocdbbackend.cpp infocdbbackend.h dbusnamelistener.h	\
	  dbusnamelistener.cpp handlesignalrouter.cpp			\
	  handlesignalrouter.h sconnect.h queuedinvoker.cpp		\
	  queuedinvoker.h logging.h logging.cpp

includecontextsubscriberdir=$(includedir)/contextsubscriber
includecontextsubscriber_HEADERS = contextproperty.h contextpropertyinfo.h contextregistryinfo.h
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtXml_CFLAGS) $(QtDBus_CFLAGS)	\
	'-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
	'-DCONTEXT_LOG_MODULE_NAME="libcontextsubscriber"'
LIBS += $(CDB_LIBS) $(QtCore_LIBS) $(QtXml_LIBS) $(QtDBus_LIBS)

# moccing
nodist_libcontextsubscriber_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextsubscriber_la_SOURCES))
include $(top_srcdir)/am/qt.am

# because if you change configure parameter DEFAULT_CONTEXT_PROVIDERS,
# you should do a recompile
infocdbbackend.lo infoxmlbackend.lo: Makefile
