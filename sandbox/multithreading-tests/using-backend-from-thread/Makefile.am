noinst_PROGRAMS = run-test
run_test_SOURCES = main.cpp thread.h

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
LIBS += $(QtCore_LIBS) $(QtDBus_LIBS)

# library dependency hack for seamless make in cli/
AM_CXXFLAGS += -I$(srcdir)/../../src
run_test_LDADD = ../../src/libcontextsubscriber.la

../../src/libcontextsubscriber.la: FORCE
	$(MAKE) -C ../../src libcontextsubscriber.la
.PHONY: FORCE

# moccing
nodist_run_test_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(run_test_SOURCES))
include $(top_srcdir)/am/qt.am
