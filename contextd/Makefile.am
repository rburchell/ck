bin_PROGRAMS = contextd

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
#$(VALAC) -C -g --basedir $(top_srcdir)/contextd --vapidir=$(top_srcdir)/vapi --pkg dbus-glib-1 --pkg gee-1.0 --pkg hal --disable-dbus-transformation $^ $(top_srcdir)/libcontextprovider/contextprovider.vapi

contextd.vala.stamp: $(contextd_VALASOURCES)
	$(VALAC) -C -g --basedir $(top_srcdir)/contextd --vapidir=$(top_srcdir)/vapi --pkg dbus-glib-1 --pkg gee-1.0 --disable-dbus-transformation $^ $(top_srcdir)/libcontextprovider/contextprovider.vapi hal.vapi
	touch contextd.vala.stamp

contextd_CFLAGS = \
	$(CONTEXTD_CFLAGS) \
	-I$(top_srcdir)/libcontextprovider \
	$(NULL)

contextd_LDADD = \
	$(CONTEXTD_LIBS) \
	-L../libcontextprovider/.libs \
	-lcontextprovider \
	-lm \
	$(NULL)

EXTRA_DIST = \
	$(contextd_GENERATEDSOURCES) \
	$(contextd_VALASOURCES) \
	contextd.vala.stamp \
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

