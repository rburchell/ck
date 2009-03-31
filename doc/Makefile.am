SUBDIRS=reference

htmldir = $(datadir)/doc/contextkit/
html_ASCIIDOCS = context-intro.txt context-providers.txt

# Support for html_ASCIIDOCS follows, do not touch.

html_DATA = $(html_ASCIIDOCS:.txt=.html) \
            $(shell awk 'match($$0, /^\[dotty,(.*)\]$$/, arr) { print arr[1] }' $(html_ASCIIDOCS))

html-local: $(html_ASCIIDOCS:.txt=.html)

%.html: %.txt
	asciidoc -f myfilter.conf $^

EXTRA_DIST = $(html_ASCIIDOCS) $(html_DATA)
MAINTAINERCLEANFILES = $(html_DATA)
