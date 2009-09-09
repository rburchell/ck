bin_PROGRAMS = test-bluez-plugin

# test's sources
test_bluez_plugin_SOURCES = testbluezplugin.cpp testbluezplugin.h

# only include these files in the coverage
COVERAGE_FILES = bluezplugin.cpp bluezinterface.cpp
# FIXME: how to tell where the coverage files are?

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) '-I$(srcdir)/../util/' '-I$(srcdir)/../../src/'
AM_LDFLAGS = $(QtDBus_LIBS) 
FROM_SOURCE =  # copy these files from the real source
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_test_bluez_plugin_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(test_bluez_plugin_SOURCES)) #$(FROM_SOURCE))
include $(top_srcdir)/am/qt.am

# Running the customer test
check-customer:
	LD_LIBRARY_PATH=$(builddir)/../../plugins/bluez/.libs/ ./test-bluez-plugin