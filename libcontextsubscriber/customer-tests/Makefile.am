SUBDIRS = update-contextkit-providers testplugins

libcontextsubscribertestsdir = $(datadir)/libcontextsubscriber-tests
dist_libcontextsubscribertests_DATA = tests.xml

coverage: covoptioncheck
	COVERAGE=1 ./runTests.sh
.PHONY: coverage

check-customer:
	./runTests.sh

clean-local:
	-$(MAKE) -C coverage-build clean
	rm -rf coverage

CLEANFILES = *.pyc

EXTRA_DIST = asynchronicity commander subscription runTests.sh	\
             registry pluginchanging env.sh coverage-build      \
             waitforsilence

include $(top_srcdir)/am/covoptioncheck.am
