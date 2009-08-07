DISTCHECK_CONFIGURE_FLAGS = --disable-doc --disable-vala --disable-coverage

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	libcontextprovider \
	libcontextsubscriber \
	contextd \
	spec \
	doc \
	python \
	tools \
	fake-provider \
	common

DISTCLEANFILES = lcov.info lcov.html
MAINTAINERCLEANFILES = INSTALL

check-sum:
	$(MAKE) -C libcontextsubscriber/unit-tests check-sum

check-customer:
	$(MAKE) -C libcontextsubscriber/customer-tests check-customer

coverage: covoptioncheck
	$(MAKE) -C libcontextsubscriber/unit-tests coverage \
	&& $(MAKE) -C libcontextprovider/unit-tests coverage

.PHONY: coverage check-customer check-sum

include $(top_srcdir)/am/covoptioncheck.am
