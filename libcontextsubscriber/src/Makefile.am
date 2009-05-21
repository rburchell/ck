lib_LTLIBRARIES = libcontextsubscriber.la

libcontextsubscriber_la_SOURCES = contextproperty.cpp			\
	  contextproperty.h propertyhandle.cpp propertyhandle.h		\
	  propertyprovider.cpp propertyprovider.h managerinterface.h	\
	  managerinterface.cpp safedbuspendingcallwatcher.h		\
	  subscriberinterface.h subscriberinterface.cpp			\
	  contextpropertyinfo.cpp contextpropertyinfo.h			\
	  contextregistryinfo.cpp contextregistryinfo.h infobackend.h	\
	  infobackend.cpp infoxmlbackend.h infoxmlbackend.cpp		\
	  infoxmlkeysfinder.h			                        \
	  infoxmlkeysfinder.cpp infokeydata.h cdbwriter.h		\
	  cdbwriter.cpp cdbreader.cpp cdbreader.h infocdbbackend.cpp	\
	  infocdbbackend.h dbusnamelistener.h dbusnamelistener.cpp	\
	  handlesignalrouter.cpp handlesignalrouter.h sconnect.h

libcontextsubscriberdir=$(includedir)/contextsubscriber

libcontextsubscriber_HEADERS = contextproperty.h contextpropertyinfo.h contextregistryinfo.h

AM_CXXFLAGS = '-DDEFAULT_REGISTRY_PATH="@DEFAULT_REGISTRY_PATH@"'

# you have to add your ui designer designed forms here
# QT_FORMS = commandview.ui
# and for every form you have to specify the objects which are dependent on that
# commandview.$(OBJEXT): ui_commandview.h

LIBS += -lcdb

# LDADD = 
# noinst_LIBRARIES = 

# you can moc every header if you want to be fool proof
QT_TOMOC_H = $(filter %.h, $(libcontextsubscriber_la_SOURCES))

# or you can explicitly specify the list of headers which should be MOC'd
# QT_TOMOC_H = ...

# if you have to moc a cpp file, don't forget to mention the extra dependency
# QT_TOMOC_CXX = signallingmapunittest.cpp
#testclass.o: moc_testclass_cpp.cpp

# finally, do the moccing
CLEANFILES =
include $(top_srcdir)/am/qt.am
libcontextsubscriber_la_DEPENDENCIES = $(qt_mocobjects)
libcontextsubscriber_la_LIBADD = $(qt_mocobjects)
