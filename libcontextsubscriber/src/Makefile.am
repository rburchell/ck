lib_LTLIBRARIES = libcontextsubscriber.la

libcontextsubscriber_la_SOURCES = contextproperty.cpp			    \
	  contextproperty.h contextproperty_priv.h keymanager.cpp	    \
	  keymanager.h propertyhandle.cpp propertyhandle.h		        \
	  subscriberiface_p.h manageriface_p.h manageriface.cpp		    \
	  subscriberiface.cpp contextpropertymeta.cpp                   \
      contextpropertymeta.h contextregistry.cpp contextregistry.h

AM_CXXFLAGS = -DPREFIX="\"$(prefix)\""

# you have to add your ui designer designed forms here
# QT_FORMS = commandview.ui
# and for every form you have to specify the objects which are dependent on that
# commandview.$(OBJEXT): ui_commandview.h

LIBS = -lcdb libmocs.a

CLEANFILES =
LDADD = 
noinst_LIBRARIES = 

# you can moc every header if you want to be fool proof
QT_TOMOC_H = $(filter %.h, $(libcontextsubscriber_la_SOURCES))

# or you can explicitly specify the list of headers which should be MOC'd
# QT_TOMOC_H = ...

# if you have to moc a cpp file, don't forget to mention the extra dependency
# QT_TOMOC_CXX = signallingmapunittest.cpp
#testclass.o: moc_testclass_cpp.cpp

# finally, do the moccing
include $(top_srcdir)/am/qt.am
