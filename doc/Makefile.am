SUBDIRS=reference

htmldir = $(datadir)/doc/contextkit/html/
html_ASCIIDOCS = context-intro.txt context-providers.txt context-types.txt use-cases.txt

# Support for html_ASCIIDOCS and html_EXTRA

html_DATA = $(html_EXTRA) \
	    $(html_ASCIIDOCS:.txt=.html) \
            $(shell awk 'match($$0, /^\[dotty,(.*)\]$$/, arr) { print arr[1] }' $(html_ASCIIDOCS))

html-local: $(html_EXTRA) $(html_ASCIIDOCS:.txt=.html)

%.html: %.txt
	asciidoc -a toc --unsafe -f myfilter.conf $^

EXTRA_DIST = $(html_ASCIIDOCS) \
             $(html_DATA) \
             myfilter.conf

MAINTAINERCLEANFILES = $(html_DATA)
