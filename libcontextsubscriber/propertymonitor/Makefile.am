all:
	$(srcdir)/generate-tests-xml.sh $(top_srcdir)/spec/core.context $(srcdir)/tests.xml.begin $(srcdir)/tests.xml.middle $(srcdir)/tests.xml.end

clean-local:
	rm tests.xml

EXTRA_DIST = generate-tests-xml.sh has_value.py has_provider.py tests.xml.begin tests.xml.end tests.xml.middle
