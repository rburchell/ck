SUBDIRS = client subscription value-changes types c-api service

coverage: covoptioncheck
	COVERAGE=1 ./runTests.sh
.PHONY: coverage

clean-local:
	-$(MAKE) -C coverage-build clean
	rm -rf coverage

check-customer:
	./runTests.sh

libcontextprovidertestsdir = $(datadir)/libcontextprovider-tests
dist_libcontextprovidertests_DATA = tests.xml

EXTRA_DIST = coverage-build runTests.sh multiprovider

include $(top_srcdir)/am/covoptioncheck.am
