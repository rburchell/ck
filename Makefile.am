DISTCHECK_CONFIGURE_FLAGS = --disable-doc --disable-vala --disable-coverage

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	libcontextprovider \
	libcontextsubscriber \
	spec \
	tests \
	doc \
	python \
	tools \
	fake-provider

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc

EXTRA_DIST = contextprovider-1.0-uninstalled.pc
DISTCLEANFILES = lcov.info lcov.html
MAINTAINERCLEANFILES = INSTALL

check-sum:
	$(MAKE) -C libcontextsubscriber/unit-tests check-sum

check-customer:
	$(MAKE) -C libcontextsubscriber/customer-tests check-customer

coverage: covoptioncheck
	$(MAKE) -C libcontextsubscriber/unit-tests coverage

.PHONY: coverage check-customer check-sum

include $(top_srcdir)/am/covoptioncheck.am
