include Makefile.gtester

VALA_PACKAGES = --pkg dbus-glib-1 --pkg gee-1.0
VALA_OPTIONS = -C -g --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi $(VALA_PACKAGES)

TEST_PROGS +=							\
	test_intset						\
	test_stringset						\
	test_value_compare					\
	test_group						\
	test_grouplist						\
	test_key_usage_counter					\
	test_subscriber						\
	test_manager						\
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

BUILT_SOURCES =							\
	test_intset.vala.stamp					\
	test_stringset.vala.stamp				\
	test_value_compare.vala.stamp				\
	test_group.vala.stamp					\
	test_grouplist.vala.stamp				\
	test_key_usage_counter.vala.stamp			\
	test_subscriber.vala.stamp				\
	test_manager.vala.stamp					\
	$(NULL)

test_intset_VALASOURCES =					\
	test_intset.vala					\
	$(NULL)

test_intset_SOURCES = \
	$(test_intset_VALASOURCES:.vala=.c)			\
	$(NULL)

test_intset.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi  $(test_intset_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@

test_intset_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_intset_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_stringset_VALASOURCES = \
	test_stringset.vala					\
	$(NULL)

test_stringset_SOURCES =					\
	$(test_stringset_VALASOURCES:.vala=.c)			\
	$(NULL)

test_stringset.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi $(test_stringset_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@

test_stringset_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_stringset_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la


test_value_compare_VALASOURCES =				\
	test_value_compare.vala					\
	$(NULL)

test_value_compare_SOURCES = \
	$(test_value_compare_VALASOURCES:.vala=.c)		\
	$(NULL)

test_value_compare.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi $(test_value_compare_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@

test_value_compare_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_value_compare_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_group_VALASOURCES =					\
	test_group.vala						\
	$(NULL)

test_group_SOURCES = \
	$(test_group_VALASOURCES:.vala=.c)			\
	$(NULL)

test_group.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi $(test_group_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@


test_group_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_group_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_grouplist_VALASOURCES =					\
	test_grouplist.vala					\
	$(NULL)

test_grouplist_SOURCES = \
	$(test_grouplist_VALASOURCES:.vala=.c)			\
	$(NULL)

test_grouplist.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi $(test_grouplist_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@


test_grouplist_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_grouplist_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_key_usage_counter_VALASOURCES = \
	test_key_usage_counter.vala				\
	$(NULL)

test_key_usage_counter_SOURCES = \
	$(test_key_usage_counter_VALASOURCES:.vala=.c)		\
	$(NULL)

test_key_usage_counter.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi $(test_key_usage_counter_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@

test_key_usage_counter_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_key_usage_counter_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_subscriber_VALASOURCES =					\
	test_subscriber.vala					\
	$(NULL)

test_subscriber_SOURCES =					\
	$(test_subscriber_VALASOURCES:.vala=.c)			\
	$(NULL)

test_subscriber.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi $(test_subscriber_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@

test_subscriber_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_subscriber_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_manager_VALASOURCES =					\
	test_manager.vala					\
	$(NULL)

test_manager_SOURCES =						\
	$(test_manager_VALASOURCES:.vala=.c)			\
	$(NULL)

test_manager.vala.stamp: $(top_srcdir)/libcontextprovider/contextprovider-internal.vapi $(test_manager_VALASOURCES)
	$(VALAC) $(VALA_OPTIONS) $^
	touch $@

test_manager_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS)
test_manager_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

EXTRA_DIST +=							\
	$(test_intset_VALASOURCES)				\
	$(test_intset_SOURCES)					\
	test_intset.vala.stamp					\
	$(test_stringset_VALASOURCES)				\
	$(test_stringset_SOURCES)				\
	test_stringset.vala.stamp				\
	$(test_value_compare_VALASOURCES)				\
	$(test_value_compare_SOURCES)				\
	test_value_compare.vala.stamp				\
	$(test_group_VALASOURCES)				\
	$(test_group_SOURCES)					\
	test_group.vala.stamp					\
	$(test_grouplist_VALASOURCES)				\
	$(test_grouplist_SOURCES)				\
	test_grouplist.vala.stamp				\
	$(test_key_usage_counter_VALASOURCES)			\
	$(test_key_usage_counter_SOURCES)			\
	test_key_usage_counter.vala.stamp			\
	$(test_subscriber_VALASOURCES)				\
	$(test_subscriber_SOURCES)				\
	test_subscriber.vala.stamp				\
	$(test_manager_VALASOURCES)				\
	$(test_manager_SOURCES)					\
	test_manager.vala.stamp					\
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

MAINTAINERCLEANFILES =						\
	$(test_intset_SOURCES)					\
	test_intset.vala.stamp					\
	$(test_stringset_SOURCES)				\
	test_stringset.vala.stamp				\
	$(test_value_compare_SOURCES)				\
	test_value_compare.vala.stamp				\
	$(test_group_SOURCES)					\
	test_group.vala.stamp					\
	$(test_grouplist_SOURCES)				\
	test_grouplist.vala.stamp				\
	$(test_key_usage_counter_SOURCES)			\
	test_key_usage_counter.vala.stamp			\
	$(test_subscriber_SOURCES)				\
	test_subscriber.vala.stamp				\
	$(test_manager_SOURCES)					\
	test_manager.vala.stamp					\
	$(NULL)
