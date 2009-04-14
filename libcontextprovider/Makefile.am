lib_LTLIBRARIES = libcontextprovider.la

libcontextprovider_VALASOURCES = \
	dbus_interface.vala \
	manager.vala \
	grouplist.vala \
	key_usage_counter.vala \
	subscriber.vala \
	value_compare.vala \
	group.vala \
	string_set.vala \
	context_provider.vala \
	$(NULL)

libcontextprovider_VAPISOURCES = \
	intset.vapi \
	$(NULL)

libcontextprovider_GENERATEDSOURCES_PRIV = \
	$(libcontextprovider_VALASOURCES:.vala=.c) \
	context_provider-internal.h \
	$(NULL)

libcontextprovider_GENERATEDSOURCES_PUB = \
	context_provider.h \
	$(NULL)

libcontextprovider_GENERATEDSOURCES = \
	$(libcontextprovider_GENERATEDSOURCES_PUB) \
	$(libcontextprovider_GENERATEDSOURCES_PRIV) \
	$(NULL)

libcontextprovider_la_SOURCES = \
	$(libcontextprovider_GENERATEDSOURCES_PRIV) \
	intset.c \
	intset.h \
	$(NULL)

libcontextproviderdir=$(includedir)/contextprovider

libcontextprovider_HEADERS = $(libcontextprovider_GENERATEDSOURCES_PUB)

libcontextprovider.vala.stamp contextprovider.vapi: $(libcontextprovider_VALASOURCES) $(libcontextprovider_VAPISOURCES)
	$(VALAC) -C -g --basedir $(top_srcdir)/libcontextprovider --vapidir=$(top_srcdir)/vapi --pkg dbus-glib-1 --pkg gee-1.0 --disable-dbus-transformation --library contextprovider --header context_provider.h --internal-header context_provider-internal.h --internal-vapi contextprovider-internal.vapi $^
	touch libcontextprovider.vala.stamp

libcontextprovider_la_CFLAGS =	\
				-DG_LOG_DOMAIN=\"ContextKit\" \
				$(CONTEXTD_CFLAGS) \
				$(NULL)

libcontextprovider_la_LIBADD = \
			  $(CONTEXTD_LIBS) \
			  $(NULL)

EXTRA_DIST = \
	$(libcontextprovider_GENERATEDSOURCES) \
	$(libcontextprovider_VALASOURCES) \
	$(libcontextprovider_VAPISOURCES) \
	libcontextprovider.vala.stamp \
	contextprovider.vapi \
	contextprovider-internal.vapi \
	$(NULL)

DISTCLEANFILES = \
	*.gcno \
	*.gcda \
	$(NULL)

MAINTAINERCLEANFILES = \
	$(libcontextprovider_GENERATEDSOURCES) \
	libcontextprovider.vala.stamp \
	$(NULL)

BUILT_SOURCES = \
	libcontextprovider.vala.stamp \
	$(NULL)

