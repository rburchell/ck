DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

# We exclude the caches from the uninstall check.
# fixme: add this back for make distcheck
# distuninstallcheck_listfiles = find . -type f -a ! -name '*.cdb' -print

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
	vapi

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc

EXTRA_DIST = contextprovider-1.0-uninstalled.pc
DISTCLEANFILES = lcov.info lcov.html
MAINTAINERCLEANFILES = INSTALL

#if HAVE_COVERAGE
#include tools/lcov.am
#endif
