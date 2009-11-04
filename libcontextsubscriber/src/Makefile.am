lib_LTLIBRARIES = libcontextsubscriber.la
libcontextsubscriber_la_SOURCES = contextproperty.cpp			\
	contextproperty.h propertyhandle.cpp propertyhandle.h		\
	provider.cpp provider.h safedbuspendingcallwatcher.h		\
	subscriberinterface.h subscriberinterface.cpp			\
	contextpropertyinfo.cpp contextpropertyinfo.h			\
	contextregistryinfo.cpp contextregistryinfo.h infobackend.h	\
	infobackend.cpp infoxmlbackend.h infoxmlbackend.cpp		\
	infokeydata.h cdbwriter.h cdbwriter.cpp cdbreader.cpp		\
	cdbreader.h infocdbbackend.cpp infocdbbackend.h			\
	dbusnamelistener.h dbusnamelistener.cpp handlesignalrouter.cpp	\
	handlesignalrouter.h queuedinvoker.cpp queuedinvoker.h		\
	loggingfeatures.h contextkitplugin.h contextkitplugin.cpp	\
	iproviderplugin.h contextproviderinfo.h nanoxml.h nanoxml.cpp	\
	asyncdbusinterface.cpp timedvalue.h nanotree.h nanotree.cpp	\
	contexttypeinfo.cpp contexttypeinfo.h				\
	contexttyperegistryinfo.cpp contexttyperegistryinfo.h

includecontextsubscriberdir=$(includedir)/contextsubscriber
includecontextsubscriber_HEADERS = contextproperty.h			\
	contextpropertyinfo.h contextregistryinfo.h iproviderplugin.h	\
	contextproviderinfo.h asyncdbusinterface.h contexttypeinfo.h	\
	contexttypeinfo.cpp contexttyperegistryinfo.h nanotree.h

AM_CXXFLAGS = -I$(top_srcdir)/common \
	      $(QtCore_CFLAGS) $(QtXml_CFLAGS) $(QtDBus_CFLAGS)	\
	      '-DDEFAULT_CONTEXT_SUBSCRIBER_PLUGINS="@libdir@/contextkit/subscriber-plugins"' \
	      '-DDEFAULT_CONTEXT_PROVIDERS="@datadir@/contextkit/providers/"' \
	      '-DDEFAULT_CONTEXT_CORE_DECLARATIONS="@datadir@/contextkit/core.context"' \
	      '-DDEFAULT_CONTEXT_TYPES="@datadir@/contextkit/types/"' \
	      '-DDEFAULT_CONTEXT_CORE_TYPES="@datadir@/contextkit/types/core.types"' \
	      '-DCONTEXT_LOG_MODULE_NAME="libcontextsubscriber"'

$(top_builddir)/common/libcommon.la: FORCE
	$(MAKE) -C $(top_builddir)/common libcommon.la

LIBS += $(CDB_LIBS) $(QtCore_LIBS) $(QtXml_LIBS) $(QtDBus_LIBS)
libcontextsubscriber_la_LIBADD=$(top_builddir)/common/libcommon.la

.PHONY: FORCE

# moccing
nodist_libcontextsubscriber_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextsubscriber_la_SOURCES))
include $(top_srcdir)/am/qt.am

# because if you change configure parameter DEFAULT_CONTEXT_PROVIDERS,
# you should do a recompile
infocdbbackend.lo infoxmlbackend.lo: Makefile

# and the same for DEFAULT_CONTEXT_SUBSCRIBER_PLUGINS
provider.lo: Makefile
