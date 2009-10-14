check_PROGRAMS = provider-unit-tests

# your test's sources
provider_unit_tests_SOURCES = testprovider.cpp testprovider.h	\
	queuedinvoker.h contextkitplugin.h handlesignalrouter.h

# only include these files in the coverage
COVERAGE_FILES = provider.cpp

# do the testing, coverage, etc. stuff
# tests.am is using +=, so we have to set a value here for these four always
AM_CXXFLAGS = $(QtDBus_CFLAGS) \
	      '-DDEFAULT_CONTEXT_SUBSCRIBER_PLUGINS="@libdir@/contextkit/subscriber-plugins"'

AM_LDFLAGS = $(QtDBus_LIBS)
# copy these files from the real source
FROM_SOURCE = provider.cpp provider.h iproviderplugin.h	\
	loggingfeatures.h contextproviderinfo.h
FROM_SOURCE_DIR = $(srcdir)/../../src
LDADD =
include $(top_srcdir)/am/tests.am

nodist_provider_unit_tests_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(provider_unit_tests_SOURCES) $(FROM_SOURCE))
include $(top_srcdir)/am/qt.am
