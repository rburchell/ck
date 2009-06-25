SUBDIRS=reference

htmldir = $(datadir)/doc/contextkit/html/
html_ASCIIDOCS = context-intro.txt context-providers.txt context-types.txt context-arch.txt use-cases.txt

# Support for html_ASCIIDOCS and html_EXTRA

html_DATA = $(html_EXTRA) \
	    $(html_ASCIIDOCS:.txt=.html) \
            $(shell sed -n '/^\[dotty,/ { s/^.*,// ; s/].*// ; p }' $(html_ASCIIDOCS:%=$(srcdir)/%))

html-local: $(html_EXTRA) $(html_ASCIIDOCS:.txt=.html)

if CONTEXTKIT_BUILD_DOCS
%.html: %.txt
	asciidoc -a toc --unsafe -f myfilter.conf $^
endif

EXTRA_DIST = $(html_ASCIIDOCS) \
             $(html_DATA) \
             myfilter.conf

MAINTAINERCLEANFILES = $(html_DATA)
