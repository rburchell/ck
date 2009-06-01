bin_PROGRAMS = update-contextkit-providers

update_contextkit_providers_SOURCES = update-contextkit-providers.cpp

AM_CPPFLAGS = -I$(srcdir)/../src

# you have to add your ui designer designed forms here
# QT_FORMS = commandview.ui
# and for every form you have to specify the objects which are dependent on that
# commandview.$(OBJEXT): ui_commandview.h

# you can moc every header if you want to be fool proof
QT_TOMOC_H = $(filter %.h, $(context_cli_SOURCES))

AM_CXXFLAGS = '-DDEFAULT_CONTEXT_PROVIDERS="@DEFAULT_CONTEXT_PROVIDERS@"'

# or you can explicitly specify the list of headers which should be MOC'd
# QT_TOMOC_H = ...

# if you have to moc a cpp file, don't forget to mention the extra dependency
# QT_TOMOC_CXX = testclass1.cpp testclass2.cpp
#testclass1.o: moc_testclass1_cpp.cpp
#testclass2.o: moc_testclass2_cpp.cpp

# finally, do the moccing
CLEANFILES =
include $(top_srcdir)/am/qt.am
update_contextkit_providers_DEPENDENCIES = $(qt_moclobjects) ../src/libcontextsubscriber.la
update_contextkit_providers_LDADD = $(qt_moclobjects) ../src/libcontextsubscriber.la

# library dependency hack for seamless make in cli/
../src/libcontextsubscriber.la:
	$(MAKE) -C ../src libcontextsubscriber.la

.PHONY: ../src/libcontextsubscriber.la
