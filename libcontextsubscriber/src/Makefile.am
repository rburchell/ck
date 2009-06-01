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
	  handlesignalrouter.h sconnect.h
includecontextsubscriberdir=$(includedir)/contextsubscriber
includecontextsubscriber_HEADERS = contextproperty.h contextpropertyinfo.h contextregistryinfo.h
AM_CXXFLAGS = '-DDEFAULT_CONTEXT_PROVIDERS="@DEFAULT_CONTEXT_PROVIDERS@"'
LIBS += -lcdb

# moccing
nodist_libcontextsubscriber_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextsubscriber_la_SOURCES))
include $(top_srcdir)/am/qt-new.am
