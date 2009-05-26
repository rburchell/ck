NULL =

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	libcontextprovider \
	libcontextsubscriber \
	contextd \
	spec \
	tests \
	doc \
	tools \
	python \
	fake-provider \
	vapi \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc

EXTRA_DIST = contextprovider-1.0-uninstalled.pc

DISTCLEANFILES = \
	lcov.info \
	lcov.html \
	$(NULL)

version:
	@echo @VERSION@

.PHONEY: version

if HAVE_COVERAGE
include tools/lcov.am
endif
