SUBDIRS=reference

htmldir = $(datadir)/doc/contextkit/html/
html_ASCIIDOCS = context-intro.txt context-providers.txt
html_EXTRA = context-properties.html contextkit.html context-provider-schema.html

MAINTAINERCLEANFILES = context-properties.html contextkit.html

# Creating docs from a property declaration file

context-properties.html: $(spec_dir)/context.xml context2asciidoc.pl
	$(srcdir)/context2asciidoc.pl < $< | asciidoc -f myfilter.conf -o $@ -

# Creating docs from a DbusSpec

spec_dir = $(top_srcdir)/spec
SPEC_FILES = $(addprefix $(spec_dir)/, \
               $(shell awk 'match($$0, /<xi:include href="(.*)"\/>/, arr) { print arr[1] }' $(spec_dir)/all.xml))

XSLTPROC = xsltproc --xinclude --nonet

contextkit.html: $(spec_dir)/all.xml $(SPEC_FILES) doc-generator.xsl
	$(XSLTPROC) $(srcdir)/doc-generator.xsl $< > $@.tmp && mv $@.tmp $@

# Creating docs from a XML schema

context-provider-schema.html: $(spec_dir)/context.xsd
	xsltproc --nonet --output $@ xs3p/xs3p.xsl $<

# Support for html_ASCIIDOCS and html_EXTRA

html_DATA = $(html_EXTRA) \
	    $(html_ASCIIDOCS:.txt=.html) \
            $(shell awk 'match($$0, /^\[dotty,(.*)\]$$/, arr) { print arr[1] }' $(html_ASCIIDOCS))

html-local: $(html_EXTRA) $(html_ASCIIDOCS:.txt=.html)

%.html: %.txt
	asciidoc -f myfilter.conf $^

EXTRA_DIST = $(html_ASCIIDOCS) $(html_DATA) doc-generator.xsl xs3p
MAINTAINERCLEANFILES += $(html_DATA)
