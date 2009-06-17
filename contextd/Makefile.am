bin_PROGRAMS = contextd

contextddir = $(datadir)/context-providers
contextd_DATA = contextd.xml

providersdir = $(datadir)/contextkit/providers
dist_providers_DATA = org.freedesktop.ContextKit.contextd.context

servicedir = $(datadir)/dbus-1/services
service_DATA = org.freedesktop.ContextKit.contextd.service

contextd_VALASOURCES = main.vala plugin.vala hal_plugin.vala	\
	lowmem_plugin.vala

contextd_GENERATEDSOURCES = $(contextd_VALASOURCES:.vala=.c)
contextd_SOURCES = $(contextd_GENERATEDSOURCES)

# FIXME: Remove the hal.vapi once the official version includes the needed functions
contextd.vala.stamp: $(top_srcdir)/vapi/hal.vapi $(top_srcdir)/vapi/posix.vapi $(top_srcdir)/libcontextprovider/contextprovider.vapi $(contextd_VALASOURCES)
	$(VALAC) -C -g --basedir $(top_srcdir)/contextd --vapidir=$(top_srcdir)/vapi --pkg dbus-glib-1 --pkg gee-1.0 --disable-dbus-transformation $^
	touch contextd.vala.stamp

AM_CFLAGS = $(CONTEXTD_CFLAGS) -I$(top_srcdir)/libcontextprovider

LDADD = $(CONTEXTD_LIBS)						\
	$(top_builddir)/libcontextprovider/libcontextprovider.la -lm

EXTRA_DIST = $(contextd_GENERATEDSOURCES) $(contextd_VALASOURCES)	\
	contextd.xml contextd.vala.stamp				\
	org.freedesktop.ContextKit.contextd.service

DISTCLEANFILES = *.gcno *.gcda

MAINTAINERCLEANFILES = $(contextd_GENERATEDSOURCES)	\
	contextd.vala.stamp

BUILT_SOURCES = contextd.vala.stamp

install-data-hook:
	-update-context-providers $(DESTDIR)$(contextdir)
	-update-contextkit-providers $(DESTDIR)$(providersdir)
