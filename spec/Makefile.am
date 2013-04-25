#dist_noinst_DATA = context.valid.stamp $(INTROSPECT)

#htmldir = $(datadir)/doc/contextkit-doc/html/
#dist_html_DATA = context-properties.html core-types.html contextkit.html context-provider-schema.html

contextdir = $(datadir)/contextkit/
dist_context_DATA = core.context

contexttypesdir = $(contextdir)/types/
dist_contexttypes_DATA = core.types

XSLTPROC = xsltproc --xinclude --nonet
DROP_NAMESPACE = perl -pe '$$hash = chr(35); s{xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec$${hash}extensions-v0"}{}g'
RST2HTML = rst2html
XMLLINT = xmllint --noout --schema

SPEC_TOOLS = spec-to-introspect.xsl \
             context2asciidoc.pl \
             doc-generator.xsl \
             xs3p

INTERFACES = \
	Manager.xml \
	Subscriber.xml \
	ContextKit.xml

SPEC_FILES = \
	$(INTERFACES) \
	generic-types.xml

INTROSPECT = $(INTERFACES:%.xml=introspect/%.xml)

tools_dir = $(top_srcdir)/tools

if CONTEXTKIT_BUILD_DOCS
# Generating introspection files

$(INTROSPECT): introspect/%.xml: %.xml spec-to-introspect.xsl
	install -d introspect
	$(XSLTPROC) $(srcdir)/spec-to-introspect.xsl $< | $(DROP_NAMESPACE) > $@

# Validating our core property list

context.valid.stamp: context.xsd core.context
	$(XMLLINT) $^
	touch $@

# Creating docs from a property declaration file
context-properties.txt: core.context context2asciidoc.pl
	$(srcdir)/context2asciidoc.pl <$< >$@.tmp || { rm $@.tmp; false; }
	mv $@.tmp $@
context-properties.html: context-properties.txt
	asciidoc -f $(srcdir)/myfilter.conf -o $@ $<

# Creating type docs from a type declaration file
core-types.txt: core.types context2asciidoc.pl
	$(srcdir)/context2asciidoc.pl <$< >$@.tmp || { rm $@.tmp; false; }
	mv $@.tmp $@
core-types.html: core-types.txt
	asciidoc -f $(srcdir)/myfilter.conf -o $@ $<

# Creating docs from a DbusSpec

contextkit.html: all.xml $(SPEC_FILES) doc-generator.xsl
	$(XSLTPROC) $(srcdir)/doc-generator.xsl $< > $@.tmp && mv $@.tmp $@

# Creating docs from a XML schema

context-provider-schema.html: context.xsd
	xsltproc --nonet --output $@ xs3p/xs3p.xsl $<
endif

EXTRA_DIST = \
	$(SPEC_FILES) \
	all.xml \
	context.xsd \
	$(SPEC_TOOLS) \
        myfilter.conf \
        Context.owl

MAINTAINERCLEANFILES = \
	$(INTROSPECT) \
	context.valid.stamp \
        context-properties.html \
        core-types.html \
        contextkit.html \
        context-provider-schema.html

maintainer-clean-local:
	rmdir introspect
