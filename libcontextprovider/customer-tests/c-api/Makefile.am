bin_PROGRAMS = cprovider

cprovider_SOURCES = cprovider.c

check-customer:
	PATH=$$PATH:../client CONTEXT_LOG_VERBOSITY=WARNING ./cprovider

cprovider_CFLAGS = -I$(top_srcdir)/libcontextprovider/src $(GLIB_CFLAGS) $(DBUS_CFLAGS) $(GOBJECT_CFLAGS)
cprovider_LDFLAGS = $(GLIB_LIBS) $(DBUS_LIBS) $(GOBJECT_LIBS) -L$(top_builddir)/libcontextprovider/src/.libs -lcontextprovider
