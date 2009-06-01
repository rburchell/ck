bin_PROGRAMS = context-listen
context_listen_SOURCES = context-listen.cpp sconnect.h			\
	propertylistener.h propertylistener.cpp commandwatcher.h	\
	commandwatcher.cpp

# library dependency hack for seamless make in cli/
AM_CXXFLAGS = -I$(srcdir)/../src
context_listen_LDADD = ../src/libcontextsubscriber.la
../src/libcontextsubscriber.la:
	$(MAKE) -C ../src libcontextsubscriber.la
.PHONY: ../src/libcontextsubscriber.la

# moccing
nodist_context_listen_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(context_listen_SOURCES))
include $(top_srcdir)/am/qt-new.am
