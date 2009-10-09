SUBDIRS = client subscription value-changes types c-api

coverage: covoptioncheck
	COVERAGE=1 ./runTests.sh
.PHONY: coverage

clean-local:
	$(MAKE) -C coverage-build clean
	rm -rf coverage

check-customer:
	./runTests.sh

libcontextprovidertestsdir = $(datadir)/libcontextprovider-tests
libcontextprovidertests_DATA = tests.xml

EXTRA_DIST = tests.xml

include $(top_srcdir)/am/covoptioncheck.am
