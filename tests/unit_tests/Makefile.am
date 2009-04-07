include Makefile.gtester

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

test_intset_VAPISOURCES =					\
	../../libcontextprovider/intset.vapi			\
	$(NULL)

test_intset_VALASOURCES =					\
	test_intset.vala					\
	$(NULL)

test_intset_GENERATEDSOURCES =					\
	$(test_intset_VALASOURCES:.vala=.c)			\
	$(test_intset_VALASOURCES:.vala=.h)			\
	$(NULL)

test_intset_SOURCES = \
	$(test_intset_GENERATEDSOURCES)				\
	../../libcontextprovider/intset.c			\
	../../libcontextprovider/intset.h			\
	$(NULL)

test_intset.vala.stamp: $(test_intset_VALASOURCES) $(test_intset_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) $^
	touch $@

test_intset_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_intset_LDADD = $(CONTEXTD_LIBS)

test_stringset_VAPISOURCES = \
	../../libcontextprovider/intset.vapi			\
	$(NULL)

test_stringset_VALASOURCES = \
	../../libcontextprovider/string_set.vala		\
	test_stringset.vala					\
	$(NULL)

test_stringset_GENERATEDSOURCES =				\
	$(test_stringset_VALASOURCES:.vala=.c)			\
	$(test_stringset_VALASOURCES:.vala=.h)			\
	$(NULL)

test_stringset_SOURCES =					\
	$(test_stringset_GENERATEDSOURCES)			\
	../../libcontextprovider/intset.c			\
	../../libcontextprovider/intset.h			\
	$(NULL)

test_stringset.vala.stamp: $(test_stringset_VALASOURCES) $(test_stringset_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 $^
	touch $@

test_stringset_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_stringset_LDADD = $(CONTEXTD_LIBS)


test_value_compare_VALASOURCES =				\
	../../libcontextprovider/value_compare.vala		\
	test_value_compare.vala					\
	$(NULL)

test_value_compare_SOURCES = \
	$(test_value_compare_VALASOURCES:.vala=.c)		\
	$(test_value_compare_VALASOURCES:.vala=.h)		\
	$(NULL)

test_value_compare.vala.stamp: $(test_value_compare_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 $^
	touch $@

test_value_compare_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_value_compare_LDADD = $(CONTEXTD_LIBS)

test_group_VALASOURCES =					\
	test_group.vala						\
	$(NULL)

test_group_SOURCES = \
	$(test_group_VALASOURCES:.vala=.c)			\
	$(test_group_VALASOURCES:.vala=.h)			\
	$(NULL)

test_group.vala.stamp: $(top_builddir)/libcontextprovider/contextprovider.vapi $(test_group_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@


test_group_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_group_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_grouplist_VALASOURCES =					\
	test_grouplist.vala					\
	$(NULL)

test_grouplist_SOURCES = \
	$(test_grouplist_VALASOURCES:.vala=.c)			\
	$(test_grouplist_VALASOURCES:.vala=.h)			\
	$(NULL)

test_grouplist.vala.stamp: $(top_builddir)/libcontextprovider/contextprovider.vapi $(test_grouplist_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@


test_grouplist_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_grouplist_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_key_usage_counter_VALASOURCES = \
	test_key_usage_counter.vala				\
	$(NULL)

test_key_usage_counter_SOURCES = \
	$(test_key_usage_counter_VALASOURCES:.vala=.c)		\
	$(test_key_usage_counter_VALASOURCES:.vala=.h)		\
	$(NULL)

test_key_usage_counter.vala.stamp: $(top_builddir)/libcontextprovider/contextprovider.vapi $(test_key_usage_counter_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@

test_key_usage_counter_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_key_usage_counter_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_subscriber_VAPISOURCES =					\
	../../libcontextprovider/intset.vapi			\
	$(NULL)

test_subscriber_VALASOURCES =					\
	../../libcontextprovider/value_compare.vala		\
	../../libcontextprovider/subscriber.vala		\
	../../libcontextprovider/manager.vala			\
	../../libcontextprovider/key_usage_counter.vala		\
	../../libcontextprovider/string_set.vala		\
	../../libcontextprovider/group.vala			\
	../../libcontextprovider/grouplist.vala			\
	../../libcontextprovider/dbus_interface.vala		\
	../../libcontextprovider/context_provider.vala		\
	test_subscriber.vala					\
	$(NULL)

test_subscriber_GENERATEDSOURCES =				\
	$(test_subscriber_VALASOURCES:.vala=.c)			\
	$(test_subscriber_VALASOURCES:.vala=.h)			\
	$(NULL)

test_subscriber_SOURCES =					\
	$(test_subscriber_GENERATEDSOURCES)			\
	../../libcontextprovider/intset.c			\
	../../libcontextprovider/intset.h			\
	$(NULL)

test_subscriber.vala.stamp: $(test_subscriber_VALASOURCES) $(test_subscriber_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@

test_subscriber_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_subscriber_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

test_manager_VAPISOURCES =					\
	../../libcontextprovider/intset.vapi			\
	$(NULL)

test_manager_VALASOURCES =					\
	../../libcontextprovider/value_compare.vala		\
	../../libcontextprovider/manager.vala			\
	../../libcontextprovider/subscriber.vala		\
	../../libcontextprovider/context_provider.vala		\
	../../libcontextprovider/key_usage_counter.vala		\
	../../libcontextprovider/string_set.vala		\
	../../libcontextprovider/group.vala			\
	../../libcontextprovider/grouplist.vala			\
	../../libcontextprovider/dbus_interface.vala		\
	test_manager.vala					\
	$(NULL)

test_manager_GENERATEDSOURCES =					\
	$(test_manager_VALASOURCES:.vala=.c)			\
	$(test_manager_VALASOURCES:.vala=.h)			\
	$(NULL)

test_manager_SOURCES =						\
	$(test_manager_GENERATEDSOURCES)			\
	../../libcontextprovider/intset.c			\
	../../libcontextprovider/intset.h			\
	$(NULL)

test_manager.vala.stamp: $(test_manager_VALASOURCES) $(test_manager_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@

test_manager_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
test_manager_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider/libcontextprovider.la

EXTRA_DIST +=							\
	$(test_intset_VALASOURCES)				\
	$(test_intset_VAPISOURCES)				\
	$(test_intset_GENERATEDSOURCES)				\
	test_intset.vala.stamp					\
	$(test_stringset_VALASOURCES)				\
	$(test_stringset_VAPISOURCES)				\
	$(test_stringset_GENERATEDSOURCES)			\
	test_stringset.vala.stamp				\
	$(value_compare_VALASOURCES)				\
	$(value_compare_SOURCES)				\
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
	$(test_subscriber_VAPISOURCES)				\
	$(test_subscriber_GENERATEDSOURCES)			\
	test_subscriber.vala.stamp				\
	$(test_manager_VALASOURCES)				\
	$(test_manager_VAPISOURCES)				\
	$(test_manager_GENERATEDSOURCES)			\
	test_manager.vala.stamp					\
	$(NULL)

DISTCLEANFILES = \
	$(NULL)

MAINTAINERCLEANFILES =						\
	$(test_intset_GENERATEDSOURCES)				\
	test_intset.vala.stamp					\
	$(test_stringset_GENERATEDSOURCES)			\
	test_stringset.vala.stamp				\
	$(test_value_compare_SOURCES)				\
	test_value_compare.vala.stamp				\
	$(test_group_SOURCES)					\
	test_group.vala.stamp					\
	$(test_grouplist_SOURCES)				\
	test_grouplist.vala.stamp				\
	$(test_key_usage_counter_SOURCES)			\
	test_key_usage_counter.vala.stamp			\
	$(test_subscriber_GENERATEDSOURCES)			\
	test_subscriber.vala.stamp				\
	$(test_manager_GENERATEDSOURCES)			\
	test_manager.vala.stamp					\
	$(NULL)
