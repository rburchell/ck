lib_LTLIBRARIES = libcontextprovider.la

libcontextprovider_VALASOURCES = dbus_interface.vala manager.vala	\
	grouplist.vala key_usage_counter.vala subscriber.vala		\
	value_compare.vala group.vala string_set.vala			\
	context_provider.vala intset_binding.vala

libcontextprovider_GENERATEDSOURCES_PRIV =		\
	$(libcontextprovider_VALASOURCES:.vala=.c)	\
	context_provider-internal.h

libcontextprovider_GENERATEDSOURCES_PUB = context_provider.h

libcontextprovider_GENERATEDSOURCES =			\
	$(libcontextprovider_GENERATEDSOURCES_PUB)	\
	$(libcontextprovider_GENERATEDSOURCES_PRIV)

libcontextprovider_la_SOURCES = intset.c intset.h	\
	$(libcontextprovider_GENERATEDSOURCES_PRIV)

libcontextproviderdir=$(includedir)/contextprovider

libcontextprovider_HEADERS = $(libcontextprovider_GENERATEDSOURCES_PUB)

libcontextprovider.vala.stamp contextprovider.vapi: $(libcontextprovider_VALASOURCES)
	$(VALAC) -C -g --basedir $(top_srcdir)/libcontextprovider \
		--vapidir=$(top_srcdir)/vapi --pkg dbus-glib-1 --pkg gee-1.0 \
	        --disable-dbus-transformation --library contextprovider \
                --header context_provider.h --internal-header context_provider-internal.h \
	        --internal-vapi contextprovider-internal.vapi $^
	touch libcontextprovider.vala.stamp

AM_CFLAGS = -DG_LOG_DOMAIN=\"ContextKit\" $(CONTEXTPROVIDER_CFLAGS)

AM_LDFLAGS = $(CONTEXTPROVIDER_LIBS)

EXTRA_DIST = $(libcontextprovider_GENERATEDSOURCES)		\
	$(libcontextprovider_VALASOURCES)			\
	libcontextprovider.vala.stamp contextprovider.vapi	\
	contextprovider-internal.vapi

DISTCLEANFILES = *.gcno *.gcda

MAINTAINERCLEANFILES = $(libcontextprovider_GENERATEDSOURCES)	\
	libcontextprovider.vala.stamp contextprovider.vapi	\
	contextprovider-internal.vapi

BUILT_SOURCES = libcontextprovider.vala.stamp

