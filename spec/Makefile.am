NULL =

dist_noinst_DATA = context.valid.stamp $(INTROSPECT)

htmldir = $(datadir)/doc/contextkit/html/
dist_html_DATA = context-properties.html contextkit.html context-provider-schema.html

XSLTPROC = xsltproc --xinclude --nonet
DROP_NAMESPACE = perl -pe '$$hash = chr(35); s{xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec$${hash}extensions-v0"}{}g'
RST2HTML = rst2html
XMLLINT = xmllint --noout --schema

SPEC_TOOLS = spec-to-introspect.xsl \
             context2asciidoc.pl \
             doc-generator.xsl

INTERFACES = \
	Manager.xml \
	Subscriber.xml \
	$(NULL)

SPEC_FILES = \
	$(INTERFACES) \
	generic-types.xml \
	$(NULL)

INTROSPECT = $(INTERFACES:%.xml=introspect/%.xml)

tools_dir = $(top_srcdir)/tools

# Generating introspection files

$(INTROSPECT): introspect/%.xml: %.xml spec-to-introspect.xsl
	install -d introspect
	$(XSLTPROC) $(srcdir)/spec-to-introspect.xsl $< | $(DROP_NAMESPACE) > $@

# Validating our core property list

context.valid.stamp: context.xsd context.xml
	$(XMLLINT) $^
	touch $@

# Creating docs from a property declaration file

context-properties.html: context.xml context2asciidoc.pl
	$(srcdir)/context2asciidoc.pl < $< | asciidoc -f $(srcdir)/myfilter.conf -o $@ -

# Creating docs from a DbusSpec

contextkit.html: all.xml $(SPEC_FILES) doc-generator.xsl
	$(XSLTPROC) $(srcdir)/doc-generator.xsl $< > $@.tmp && mv $@.tmp $@

# Creating docs from a XML schema

context-provider-schema.html: context.xsd
	xsltproc --nonet --output $@ xs3p/xs3p.xsl $<

EXTRA_DIST = \
	$(SPEC_FILES) \
	all.xml \
	context.xml \
	context.xsd \
	$(SPEC_TOOLS) \
        myfilter.conf \
	$(NULL)

MAINTAINERCLEANFILES = \
	$(INTROSPECT) \
	context.valid.stamp \
        context-properties.html \
        contextkit.html \
        context-provider-schema.html \
	$(NULL)

