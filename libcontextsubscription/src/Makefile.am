lib_LTLIBRARIES = libcontextsubscription.la

libcontextsubscription_la_SOURCES = testclass.h testclass.cpp

# you have to add your ui designer designed forms here
# QT_FORMS = commandview.ui
# and for every form you have to specify the objects which are dependent on that
# commandview.$(OBJEXT): ui_commandview.h

CLEANFILES =
LDADD = 
noinst_LIBRARIES = 

# you can moc every header if you want to be fool proof
# QT_TOMOC_H = $(filter %.h, $(libcontextsubscription_SOURCES))

# or you can explicitly specify the list of headers which should be MOC'd
# QT_TOMOC_H = ...

# if you have to moc a cpp file, don't forget to mention the extra dependency
# QT_TOMOC_CXX = signallingmapunittest.cpp
# signallingmapunittest.o: moc_signallingmapunittest_cpp.cpp

# finally, do the moccing
include $(top_srcdir)/am/qt.am
