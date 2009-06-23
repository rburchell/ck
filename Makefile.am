DISTCHECK_CONFIGURE_FLAGS = --disable-doc --disable-vala

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
