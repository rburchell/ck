NULL =

AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = \
	$(NULL)

AM_CPPFLAGS = \
	$(CONTEXTD_CFLAGS) \
	-include $(CONFIG_HEADER) \
	$(NULL)

BUILT_SOURCES = src/contextd.vala.stamp $(spec_gen)

bin_PROGRAMS = contextd

contextd_VALASOURCES = \
	src/main.vala \
	src/DBusInterface.vala \
	src/Manager.vala \
	$(NULL)

contextd_SOURCES = \
	$(contextd_VALASOURCES:.vala=.c) \
	$(contextd_VALASOURCES:.vala=.h) \
	$(NULL)

src/contextd.vala.stamp: $(contextd_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg sqlite3 --pkg dbus-glib-1 $^
	touch $@

spec_gen = spec/introspect.xml doc/contextkit.html
tools_dir = $(top_srcdir)/tools

spec/introspect.xml: spec/contextkit.xml $(tools_dir)/spec-to-introspect.xsl
	$(XSLTPROC) $(tools_dir)/spec-to-introspect.xsl $< > $@

doc/contextkit.html: spec/contextkit.xml $(tools_dir)/doc-generator.xsl
	$(XSLTPROC) $(tools_dir)/doc-generator.xsl $< > $@

CLEANFILES = $(spec_gen)

contextd_LDADD = \
	$(CONTEXTD_LIBS) \
	$(NULL)

EXTRA_DIST = \
	$(contextd_VALASOURCES) \
	src/contextd.vala.stamp \
	$(NULL)

DISTCLEANFILES = \
	$(contextd_SOURCES) \
	src/contextd.vala.stamp \
	$(NULL)

