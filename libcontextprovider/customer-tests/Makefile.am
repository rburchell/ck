SUBDIRS = client subscription value-changes types c-api

check-customer:
	$(MAKE) -C subscription check-customer && $(MAKE) -C value-changes check-customer && $(MAKE) -C types check-customer && $(MAKE) -C c-api check-customer

libcontextprovidertestsdir = $(datadir)/libcontextprovider-tests
libcontextprovidertests_DATA = tests.xml

EXTRA_DIST = tests.xml