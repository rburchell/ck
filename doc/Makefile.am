htmldir = $(datadir)/doc/contextkit/html/
html_ASCIIDOCS = context-intro.txt context-providers.txt context-types.txt context-arch.txt use-cases.txt

# Support for html_ASCIIDOCS and html_EXTRA

html_DATA = $(html_EXTRA) \
	    $(html_ASCIIDOCS:.txt=.html) \
	    fig3.png fig4.png

html-local: $(html_EXTRA) $(html_ASCIIDOCS:.txt=.html)

if CONTEXTKIT_BUILD_DOCS
context-arch.html: context-arch.txt

%.html: %.txt
	asciidoc -a toc --unsafe -f myfilter.conf $^
endif

EXTRA_DIST = $(html_ASCIIDOCS) \
             $(html_DATA) \
             myfilter.conf \
             context-cron.txt \
             context-talk.txt \
             protocol-suggestion.txt \
             lgpl-2.1.txt

MAINTAINERCLEANFILES =  $(html_EXTRA) \
			$(html_ASCIIDOCS:.txt=.html)
