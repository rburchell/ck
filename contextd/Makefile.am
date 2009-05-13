bin_PROGRAMS = contextd

contextddir = $(datadir)/context-providers
contextd_DATA = contextd.xml

servicedir = $(datadir)/dbus-1/services
service_DATA = org.freedesktop.ContextKit.contextd.service

contextd_VALASOURCES = \
	main.vala \
	plugin.vala \
	hal_plugin.vala \
	$(NULL)

contextd_GENERATEDSOURCES = \
	$(contextd_VALASOURCES:.vala=.c) \
	$(NULL)

contextd_SOURCES = \
	$(contextd_GENERATEDSOURCES) \
	$(NULL)

# FIXME: Remove the hal.vapi once the official version includes the needed functions
contextd.vala.stamp: $(top_srcdir)/vapi/hal.vapi $(top_srcdir)/vapi/posix.vapi $(top_srcdir)/libcontextprovider/contextprovider.vapi $(contextd_VALASOURCES)
	$(VALAC) -C -g --basedir $(top_srcdir)/contextd --vapidir=$(top_srcdir)/vapi --pkg dbus-glib-1 --pkg gee-1.0 --disable-dbus-transformation $^
	touch contextd.vala.stamp

contextd_CFLAGS = \
	$(CONTEXTD_CFLAGS) \
	-I$(top_srcdir)/libcontextprovider \
	$(NULL)

contextd_LDADD = \
	$(CONTEXTD_LIBS) \
	$(top_builddir)/libcontextprovider/libcontextprovider.la \
	-lm \
	$(NULL)

EXTRA_DIST = \
	$(contextd_GENERATEDSOURCES) \
	$(contextd_VALASOURCES) \
	contextd.xml \
	contextd.vala.stamp \
	org.freedesktop.ContextKit.contextd.service \
	$(NULL)

DISTCLEANFILES = \
	*.gcno \
	*.gcda \
	$(NULL)

MAINTAINERCLEANFILES = \
	$(contextd_GENERATEDSOURCES) \
	contextd.vala.stamp \
	$(NULL)

BUILT_SOURCES = \
	contextd.vala.stamp \
	$(NULL)

