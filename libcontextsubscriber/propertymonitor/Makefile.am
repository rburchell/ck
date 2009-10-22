XSLTPROC = xsltproc --xinclude --nonet

all-local: tests.xml

# Generating tests.xml
tests.xml: $(top_srcdir)/spec/core.context tests.xsl
	$(XSLTPROC) $(srcdir)/tests.xsl $< >$@

EXTRA_DIST = tests.xml has_value.py has_provider.py

MAINTAINERCLEANFILES = tests.xml
