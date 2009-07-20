bin_PROGRAMS = test-cli
test_cli_SOURCES = test-cli.cpp test-cli.h

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make in test-cli
AM_CXXFLAGS += -I$(srcdir)/../src				\
	      '-DCONTEXT_LOG_MODULE_NAME="test-cli"'
test_cli_LDADD = ../src/libcontextprovider.la
../src/libcontextprovider.la:
	$(MAKE) -C ../src libcontextprovider.la
.PHONY: ../src/libcontextprovider.la

# moccing
nodist_test_cli_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(test_cli_SOURCES))
include $(top_srcdir)/am/qt.am
