include Makefile.decl

NULL =

XSLTPROC = xsltproc --xinclude --nonet
DROP_NAMESPACE = perl -pe '$$hash = chr(35); s{xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec$${hash}extensions-v0"}{}g'
RST2HTML = rst2html
XMLLINT = xmllint --noout --schema

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = \
	$(NULL)

AM_CPPFLAGS = \
	$(CONTEXTD_CFLAGS) \
	-include $(CONFIG_HEADER) \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/libcontextprovider \
	$(NULL)

BUILT_SOURCES = \
	src/contextd.vala.stamp \
	$(INTROSPECT) \
	$(ASYNC_INTROSPECT) \
	context.valid.stamp \
	$(NULL)

if CONTEXTKIT_BUILD_DOCS
BUILT_SOURCES += $(DOCS)
endif

bin_PROGRAMS = contextd

contextd_core_VALASOURCES = \
	src/main.vala \
	src/MCE.vala \
	$(NULL)

contextd_liblocation_VALASOURCES = \
	$(NULL)

contextd_VALASOURCES = \
	$(contextd_core_VALASOURCES) \
	$(contextd_liblocation_VALASOURCES) \
	$(NULL)

contextd_GENERATEDSOURCES = \
	$(contextd_VALASOURCES:.vala=.c) \
	$(contextd_VALASOURCES:.vala=.h) \
	$(NULL)

contextd_SOURCES = \
	$(contextd_core_VALASOURCES:.vala=.c) \
	$(contextd_core_VALASOURCES:.vala=.h) \
	$(NULL)

if HAVE_LOCATION
contextd_SOURCES += \
	$(contextd_liblocation_VALASOURCES:.vala=.c) \
	$(contextd_liblocation_VALASOURCES:.vala=.h) \
	$(NULL)
endif


SPEC_TOOLS = \
	tools/doc-generator.xsl \
	tools/spec-to-introspect.xsl \
	tools/context2html.pl \
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
DOCS = doc/contextkit.html doc/context.html

src/contextd.vala.stamp $(contextd_GENERATEDSOURCES): $(contextd_VALASOURCES) $(contextd_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg posix --pkg dbus-glib-1 --pkg gee-1.0 --disable-dbus-transformation contextprovider.vapi $^
	touch $@

tools_dir = $(top_srcdir)/tools

$(INTROSPECT): introspect/%.xml: spec/%.xml $(tools_dir)/spec-to-introspect.xsl
	install -d introspect
	$(XSLTPROC) $(tools_dir)/spec-to-introspect.xsl $< | $(DROP_NAMESPACE) > $@

$(ASYNC_INTROSPECT): introspect/async/%.xml: $(SPEC_FILES) $(tools_dir)/spec-to-introspect.xsl
	install -d introspect/async
	$(XSLTPROC) $(tools_dir)/spec-to-introspect.xsl $< | $(DROP_NAMESPACE) > $@

doc/contextkit.html: $(SPEC_FILES) spec/all.xml $(tools_dir)/doc-generator.xsl
	install -d doc
	$(XSLTPROC) $(tools_dir)/doc-generator.xsl $(top_srcdir)/spec/all.xml > $@

context.valid.stamp: spec/context.xsd spec/context.xml
	$(XMLLINT) $^ 
	touch $@

doc/context.html: context.valid.stamp spec/context.xml tools/context2html.pl
	install -d doc
	$(top_srcdir)/tools/context2html.pl < $(top_srcdir)/spec/context.xml > $@

CLEANFILES = $(INTROSPECT) $(ASYNC_INTROSPECT) $(DOCS) context.valid.stamp
clean-local:
	-rm -rf introspect

if HAVE_LOCATION
contextd_LDADD = \
	$(CONTEXTD_LIBS) \
	$(LOCATION_LIBS) \
	$(NULL)
else
contextd_LDADD = \
	$(CONTEXTD_LIBS) \
	$(NULL)
endif

EXTRA_DIST += \
	$(contextd_GENERATEDSOURCES) \
	$(contextd_VALASOURCES) \
	$(contextd_VAPISOURCES) \
	src/contextd.vala.stamp \
	$(SPEC_FILES) \
	spec/all.xml \
	spec/context.xml \
	spec/context.xsd \
	$(SPEC_TOOLS) \
	$(NULL)

DISTCLEANFILES = \
	$(contextd_GENERATEDSOURCES) \
	src/contextd.vala.stamp \
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
