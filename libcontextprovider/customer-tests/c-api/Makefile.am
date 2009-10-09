testdir = $(bindir)/libcontextprovider-tests
test_PROGRAMS = cprovidertests

cprovidertests_SOURCES = cprovidertests.c

check-customer: $(test_PROGRAMS)
	PATH=../client:$$PATH CONTEXT_LOG_VERBOSITY=WARNING ./.libs/cprovidertests

cprovidertests_CFLAGS = -I$(top_srcdir)/libcontextprovider/src $(GLIB_CFLAGS) $(DBUS_CFLAGS)
cprovidertests_LDFLAGS = $(GLIB_LIBS) $(DBUS_LIBS) -L$(top_builddir)/libcontextprovider/src/.libs -lcontextprovider
