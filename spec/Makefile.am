NULL =

dist_noinst_DATA = context.valid.stamp $(INTROSPECT)

XSLTPROC = xsltproc --xinclude --nonet
DROP_NAMESPACE = perl -pe '$$hash = chr(35); s{xmlns:tp="http://telepathy\.freedesktop\.org/wiki/DbusSpec$${hash}extensions-v0"}{}g'
RST2HTML = rst2html
XMLLINT = xmllint --noout --schema

SPEC_TOOLS = spec-to-introspect.xsl 

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

EXTRA_DIST = \
	$(SPEC_FILES) \
	all.xml \
	context.xml \
	context.xsd \
	$(SPEC_TOOLS) \
	$(NULL)

MAINTAINERCLEANFILES = \
	$(INTROSPECT) \
	context.valid.stamp \
	$(NULL)
