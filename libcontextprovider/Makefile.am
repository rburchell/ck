lib_LTLIBRARIES = libcontextprovider.la

libcontextprovider_VALASOURCES_PRIV = \
	dbus_interface.vala \
	manager.vala \
	grouplist.vala \
	key_usage_counter.vala \
	subscriber.vala \
	value_compare.vala \
	group.vala \
	string_set.vala \
	$(NULL)

libcontextprovider_VALASOURCES_PUB = \
	context_provider.vala \
	$(NULL)

libcontextprovider_VALASOURCES = \
	$(libcontextprovider_VALASOURCES_PRIV) \
	$(libcontextprovider_VALASOURCES_PUB) \
	$(NULL)

libcontextprovider_VAPISOURCES = \
	intset.vapi \
	$(NULL)

libcontextprovider_GENERATEDSOURCES_PRIV = \
	$(libcontextprovider_VALASOURCES_PRIV:.vala=.c) \
	$(libcontextprovider_VALASOURCES_PRIV:.vala=.h) \
	$(libcontextprovider_VALASOURCES_PUB:.vala=.c) \
	$(NULL)

libcontextprovider_GENERATEDSOURCES_PUB = \
	$(libcontextprovider_VALASOURCES_PUB:.vala=.h) \
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

