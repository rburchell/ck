include Makefile.decl

NULL =

# disable this for now. gtk-doc and non-recursive make don't play well together.
#DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

XSLTPROC = xsltproc --xinclude --nonet
DROP_NAMESPACE = perl -pe '$$hash = chr(35); s{xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec$${hash}extensions-v0"}{}g'
RST2HTML = rst2html
XMLLINT = xmllint --noout --schema


AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	.\
	doc \
	$(NULL)

pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc

AM_CPPFLAGS = \
	$(CONTEXTD_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/libcontextprovider \
	$(NULL)

BUILT_SOURCES = \
	$(INTROSPECT) \
	$(ASYNC_INTROSPECT) \
	context.valid.stamp \
	$(NULL)

# Introspection

SPEC_TOOLS = \
	tools/spec-to-introspect.xsl \
	$(NULL)


INTERFACES = \
	spec/Manager.xml \
	spec/Subscriber.xml \
	$(NULL)


SPEC_FILES = \
	$(INTERFACES) \
	spec/generic-types.xml \
	$(NULL)

INTROSPECT = $(INTERFACES:spec/%.xml=introspect/%.xml)
ASYNC_INTROSPECT = $(INTERFACES:spec/%.xml=introspect/async/%.xml)

tools_dir = $(top_srcdir)/tools

$(INTROSPECT): introspect/%.xml: spec/%.xml $(tools_dir)/spec-to-introspect.xsl
	install -d introspect
	$(XSLTPROC) $(tools_dir)/spec-to-introspect.xsl $< | $(DROP_NAMESPACE) > $@

$(ASYNC_INTROSPECT): introspect/async/%.xml: $(SPEC_FILES) $(tools_dir)/spec-to-introspect.xsl
	install -d introspect/async
	$(XSLTPROC) $(tools_dir)/spec-to-introspect.xsl $< | $(DROP_NAMESPACE) > $@

context.valid.stamp: spec/context.xsd spec/context.xml
	$(XMLLINT) $^ 
	touch $@

# Cleaning and distributing

CLEANFILES = $(INTROSPECT) $(ASYNC_INTROSPECT) context.valid.stamp
clean-local:
	-rm -rf introspect

EXTRA_DIST += \
	$(SPEC_FILES) \
	spec/all.xml \
	spec/context.xml \
	spec/context.xsd \
	$(SPEC_TOOLS) \
	tools/git-make-dist \
	tools/autogen-helpers.sh \
	$(NULL)

DISTCLEANFILES = \
	src/*.gcov \
	src/*.gcno \
	src/*.gcda \
	$(NULL)

MAINTAINERCLEANFILES = \
	$(NULL)

clean-gcov:
	find -name "*.gcov" | xargs rm -f || true
	find -name "*.gcno" | xargs rm -f || true
	find -name "*.gcda" | xargs rm -f || true

version:
	@echo @VERSION@

.PHONEY: version

include tests/unit_tests/Makefile.fragment
include libcontextprovider/Makefile.fragment
include tests/python-test-library/Makefile.fragment
include tools/lcov.am
