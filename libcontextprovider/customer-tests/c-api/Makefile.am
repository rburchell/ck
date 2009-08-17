bin_PROGRAMS = cprovidertests

cprovidertests_SOURCES = cprovidertests.c

check-customer:
	PATH=$$PATH:../client CONTEXT_LOG_VERBOSITY=WARNING ./cprovidertests

cprovidertests_CFLAGS = -I$(top_srcdir)/libcontextprovider/src $(GLIB_CFLAGS) $(DBUS_CFLAGS)
cprovidertests_LDFLAGS = $(GLIB_LIBS) $(DBUS_LIBS) -L$(top_builddir)/libcontextprovider/src/.libs -lcontextprovider
