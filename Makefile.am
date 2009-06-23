DISTCHECK_CONFIGURE_FLAGS = --disable-doc --disable-vala --disable-coverage

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	libcontextprovider \
	libcontextsubscriber \
	contextd \
	spec \
	tests \
	doc \
	python \
	fake-provider \
	vapi

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc

EXTRA_DIST = contextprovider-1.0-uninstalled.pc
DISTCLEANFILES = lcov.info lcov.html
MAINTAINERCLEANFILES = INSTALL

check-sum:
	$(MAKE) -C libcontextsubscriber/unit-tests check-sum

check-accept:
	$(MAKE) -C libcontextsubscriber/accept-tests check-accept

coverage: covoptioncheck
	$(MAKE) -C libcontextsubscriber/unit-tests coverage

.PHONY: coverage check-accept check-sum

include $(top_srcdir)/am/covoptioncheck.am
