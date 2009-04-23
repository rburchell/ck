include ../unit_tests/Makefile.gtester

VALA_PACKAGES = --pkg dbus-glib-1 --pkg gee-1.0
VALA_OPTIONS = -C -g --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi $(VALA_PACKAGES)

TEST_PROGS += \
	test_hal_plugin \
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

BUILT_SOURCES = \
	test_hal_plugin.vala.stamp \
	$(NULL)

test_hal_plugin_VALASOURCES = \
	test_hal_plugin.vala \
	mock_libhal.vala \
	mock_contextprovider.vala \
	$(top_srcdir)/contextd/hal_plugin.vala \
	$(top_srcdir)/contextd/plugin.vala \
	$(NULL)

test_hal_plugin_SOURCES = \
	$(test_hal_plugin_VALASOURCES:.vala=.c) \
	$(NULL)

test_hal_plugin.vala.stamp: $(top_srcdir)/vapi/posix.vapi $(test_hal_plugin_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@

test_hal_plugin_CFLAGS = -I$(top_srcdir)/contextd $(CONTEXTD_CFLAGS)
test_hal_plugin_LDADD = $(CONTEXTD_LIBS) -lm

EXTRA_DIST += \
	$(test_hal_plugin_VALASOURCES) \
	$(test_hal_plugin_SOURCES) \
	test_hal_plugin.vala.stamp \
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

MAINTAINERCLEANFILES =	\
	$(test_hal_plugin_SOURCES) \
	test_hal_plugin.vala.stamp \
	$(NULL)
