include ../unit_tests/Makefile.gtester

VALA_PACKAGES = --pkg dbus-glib-1 --pkg gee-1.0
VALA_OPTIONS = -g -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi $(VALA_PACKAGES)

TEST_PROGS += \
	test_hal_plugin \
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

BUILT_SOURCES = \
	test_hal_plugin.vala.stamp \
	$(NULL)

# Real source files tested by this test
test_hal_plugin_real_VALASOURCES = \
	$(top_srcdir)/contextd/hal_plugin.vala \
	$(top_srcdir)/contextd/plugin.vala \
	$(NULL)

test_hal_plugin_copied_VALASOURCES = \
	hal_plugin.vala \
	plugin.vala \
	$(NULL)

# Files of the test program
test_hal_plugin_test_VALASOURCES = \
	test_hal_plugin.vala \
	libhal.vala \
	context_provider.vala \
	$(NULL)
# Note: libhal.vala and context_provider.vala are mock implementations

test_hal_plugin_VALASOURCES = \
	$(test_hal_plugin_real_VALASOURCES) \
	$(test_hal_plugin_test_VALASOURCES) \
	$(NULL)

test_hal_plugin_SOURCES = \
	$(test_hal_plugin_test_VALASOURCES:.vala=.c) \
	$(test_hal_plugin_copied_VALASOURCES:.vala=.c) \
	$(NULL)

test_hal_plugin.vala.stamp: $(top_srcdir)/vapi/posix.vapi $(test_hal_plugin_VALASOURCES)
	cp $(test_hal_plugin_real_VALASOURCES) .
	$(VALAC) $(VALA_OPTIONS) $(test_hal_plugin_test_VALASOURCES) $(test_hal_plugin_copied_VALASOURCES)
	touch $@
	rm $(test_hal_plugin_copied_VALASOURCES)

test_hal_plugin_CFLAGS = $(CONTEXTD_CFLAGS)
test_hal_plugin_LDADD = $(CONTEXTD_LIBS) -lm

EXTRA_DIST += \
	$(test_hal_plugin_test_VALASOURCES) \
	$(test_hal_plugin_SOURCES) \
	test_hal_plugin.vala.stamp \
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

MAINTAINERCLEANFILES =	\
	$(test_hal_plugin_SOURCES) \
	test_hal_plugin.vala.stamp \
	$(NULL)
