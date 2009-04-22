bin_PROGRAMS = contextd

contextd_VALASOURCES = \
	main.vala \
	plugin.vala \
	hal_plugin.vala \
	$(NULL)

contextd_GENERATEDSOURCES = \
	$(contextd_VALASOURCES:.vala=.c) \
	$(NULL)

contextd.vala.stamp: $(contextd_VALASOURCES)
	$(VALAC) -C -g --basedir $(top_srcdir)/contextd --vapidir=$(top_srcdir)/vapi --pkg dbus-glib-1 --pkg gee-1.0 --pkg hal --disable-dbus-transformation $^ $(top_srcdir)/libcontextprovider/contextprovider.vapi
	touch contextd.vala.stamp


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

