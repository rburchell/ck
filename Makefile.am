NULL =

XSLTPROC = xsltproc --xinclude --nonet
DROP_NAMESPACE = perl -pe '$$hash = chr(35); s{xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec$${hash}extensions-v0"}{}g'
RST2HTML = rst2html
XMLLINT = xmllint --noout --schema

AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = \
	$(NULL)

AM_CPPFLAGS = \
	$(CONTEXTD_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = \
	src/contextd.vala.stamp \
	$(INTROSPECT) \
	$(ASYNC_INTROSPECT) \
	context.valid.stamp \
	$(DOCS) \
	$(NULL)

bin_PROGRAMS = contextd

contextd_VALASOURCES = \
	src/main.vala \
	src/DBusInterface.vala \
	src/Manager.vala \
	src/MCE.vala \
	src/PluginInterface.vala \
	$(NULL)

contextd_SOURCES = \
	$(contextd_VALASOURCES:.vala=.c) \
	$(contextd_VALASOURCES:.vala=.h) \
	$(NULL)

SPEC_TOOLS = \
	tools/doc-generator.xsl \
	tools/spec-to-introspect.xsl \
	tools/context2html.pl \
	$(NULL)

SPEC_FILES = \
	spec/generic-types.xml \
	spec/Manager.xml \
	spec/Subscription.xml \
	$(NULL)

INTROSPECT = $(SPEC_FILES:spec/%.xml=introspect/%.xml)
ASYNC_INTROSPECT = $(SPEC_FILES:spec/%.xml=introspect/async/%.xml)
DOCS = doc/contextkit.html doc/context.html

src/contextd.vala.stamp: $(contextd_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg sqlite3 --pkg dbus-glib-1 $^
	touch $@

tools_dir = $(top_srcdir)/tools

$(INTROSPECT): introspect/%.xml: $(SPEC_FILES) $(tools_dir)/spec-to-introspect.xsl
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
	tools/context2html.pl < $(top_srcdir)/spec/context.xml > $@

CLEANFILES = $(INTROSPECT) $(ASYNC_INTROSPECT) $(DOCS) context.valid.stamp
clean-local:
	-rm -rf introspect

contextd_LDADD = \
	$(CONTEXTD_LIBS) \
	$(NULL)

EXTRA_DIST = \
	$(contextd_VALASOURCES) \
	src/contextd.vala.stamp \
	$(SPEC_FILES) \
	spec/all.xml \
	spec/context.xml \
	spec/context.xsd \
	$(SPEC_TOOLS) \
	$(NULL)

DISTCLEANFILES = \
	$(contextd_SOURCES) \
	src/contextd.vala.stamp \
	$(NULL)

