SUBDIRS = update-contextkit-providers testplugins

libcontextsubscribertestsdir = $(datadir)/libcontextsubscriber-tests
libcontextsubscribertests_DATA = tests.xml

coverage: covoptioncheck
	$(MAKE) -C $(top_srcdir) all
	COVERAGE=1 ./runTests.sh
.PHONY: coverage

check-customer:
	$(MAKE) -C $(top_srcdir) all
	./runTests.sh

clean-local:
	$(MAKE) -C coverage-build clean
	rm -rf coverage

CLEANFILES = *.pyc

EXTRA_DIST = asynchronicity commander subscription runTests.sh	\
             tests.xml common registry pluginchanging		\
             coverage-build/Makefile

include $(top_srcdir)/am/covoptioncheck.am
