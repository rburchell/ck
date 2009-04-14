NULL =

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	libcontextprovider \
	spec \
	tests \
	doc \
	tools \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc

clean-gcov:
	find -name "*.gcov" | xargs rm -f || true
	find -name "*.gcno" | xargs rm -f || true
	find -name "*.gcda" | xargs rm -f || true

version:
	@echo @VERSION@

.PHONEY: version

if HAVE_COVERAGE
include tools/lcov.am
endif
