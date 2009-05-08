bin_PROGRAMS = context-cli

context_cli_SOURCES = context-cli.cpp

AM_CXXFLAGS = -I../src

# you have to add your ui designer designed forms here
# QT_FORMS = commandview.ui
# and for every form you have to specify the objects which are dependent on that
# commandview.$(OBJEXT): ui_commandview.h

CLEANFILES =
LDADD = ../src/libcontextsubscriber.la
noinst_LIBRARIES = 

# you can moc every header if you want to be fool proof
QT_TOMOC_H = $(filter %.h, $(context_cli_SOURCES))

# or you can explicitly specify the list of headers which should be MOC'd
# QT_TOMOC_H = ...

# if you have to moc a cpp file, don't forget to mention the extra dependency
# QT_TOMOC_CXX = testclass1.cpp testclass2.cpp
#testclass1.o: moc_testclass1_cpp.cpp
#testclass2.o: moc_testclass2_cpp.cpp

../src/libcontextsubscriber.la:
	$(MAKE) -C ../src libcontextsubscriber.la
	
# finally, do the moccing
include $(top_srcdir)/am/qt.am
