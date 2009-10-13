testdir = $(libdir)/libcontextprovider-tests
test_PROGRAMS = client

client_SOURCES = client.cpp commandwatcher.h commandwatcher.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) -I$(top_srcdir)/common

LIBS += $(QtCore_LIBS) $(QtDBus_LIBS)

# moccing
nodist_client_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(client_SOURCES))
include $(top_srcdir)/am/qt.am
