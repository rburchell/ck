TEST_PROGS += \
	intset					\
	stringset				\
	value_compare				\
	group					\
	grouplist				\
	key_usage_counter				\
	subscriber				\
	manager				\
	$(NULL)

noinst_PROGRAMS = $(TEST_PROGS)

BUILT_SOURCES += \
	intset.vala.stamp			\
	stringset.vala.stamp			\
	value_compare.vala.stamp		\
	group.vala.stamp		\
	grouplist.vala.stamp		\
	key_usage_counter.vala.stamp		\
	subscriber.vala.stamp		\
	manager.vala.stamp		\
	$(NULL)

intset_VAPISOURCES = \
	libcontextprovider/intset.vapi               \
	$(NULL)

intset_VALASOURCES = \
	intset.vala				\
	$(NULL)

intset_GENERATEDSOURCES = \
	$(intset_VALASOURCES:.vala=.c)		\
	$(intset_VALASOURCES:.vala=.h)		\
	$(NULL)

intset_SOURCES = \
	$(intset_GENERATEDSOURCES)		\
	libcontextprovider/intset.c					\
	libcontextprovider/intset.h					\
	$(NULL)

intset.vala.stamp: $(intset_VALASOURCES) $(intset_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) $^
	touch $@

intset_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
intset_LDADD = $(CONTEXTD_LIBS)

stringset_VAPISOURCES = \
	libcontextprovider/intset.vapi				\
	$(NULL)

stringset_VALASOURCES = \
	libcontextprovider/string_set.vala				\
	stringset.vala				\
	$(NULL)

stringset_GENERATEDSOURCES = \
	$(stringset_VALASOURCES:.vala=.c)		\
	$(stringset_VALASOURCES:.vala=.h)		\
	$(NULL)

stringset_SOURCES = \
	$(stringset_GENERATEDSOURCES)		\
	libcontextprovider/intset.c					\
	libcontextprovider/intset.h					\
	$(NULL)

stringset.vala.stamp: $(stringset_VALASOURCES) $(stringset_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 $^
	touch $@

stringset_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
stringset_LDADD = $(CONTEXTD_LIBS)


value_compare_VALASOURCES = \
	libcontextprovider/value_compare.vala				\
	test_value_compare.vala			\
	$(NULL)

value_compare_SOURCES = \
	$(value_compare_VALASOURCES:.vala=.c)		\
	$(value_compare_VALASOURCES:.vala=.h)		\
	$(NULL)

value_compare.vala.stamp: $(value_compare_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 $^
	touch $@

value_compare_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
value_compare_LDADD = $(CONTEXTD_LIBS)

group_VALASOURCES = \
	test_group.vala			\
	$(NULL)

group_SOURCES = \
	$(group_VALASOURCES:.vala=.c)		\
	$(group_VALASOURCES:.vala=.h)		\
	$(NULL)

group.vala.stamp: contextprovider.vapi $(group_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@


group_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
group_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider.la

grouplist_VALASOURCES = \
	test_grouplist.vala			\
	$(NULL)

grouplist_SOURCES = \
	$(grouplist_VALASOURCES:.vala=.c)		\
	$(grouplist_VALASOURCES:.vala=.h)		\
	$(NULL)

grouplist.vala.stamp: contextprovider.vapi $(grouplist_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@


grouplist_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
grouplist_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider.la

key_usage_counter_VALASOURCES = \
	test_key_usage_counter.vala			\
	$(NULL)

key_usage_counter_SOURCES = \
	$(key_usage_counter_VALASOURCES:.vala=.c)		\
	$(key_usage_counter_VALASOURCES:.vala=.h)		\
	$(NULL)

key_usage_counter.vala.stamp: contextprovider.vapi $(key_usage_counter_VALASOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@

key_usage_counter_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
key_usage_counter_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider.la

subscriber_VAPISOURCES = \
	libcontextprovider/intset.vapi				\
	$(NULL)

subscriber_VALASOURCES = \
	libcontextprovider/value_compare.vala				\
	libcontextprovider/subscriber.vala				\
	libcontextprovider/manager.vala				\
	libcontextprovider/key_usage_counter.vala				\
	libcontextprovider/string_set.vala				\
	libcontextprovider/group.vala				\
	libcontextprovider/grouplist.vala				\
	libcontextprovider/dbus_interface.vala				\
	libcontextprovider/context_provider.vala				\
	test_subscriber.vala			\
	$(NULL)

subscriber_GENERATEDSOURCES = \
	$(subscriber_VALASOURCES:.vala=.c)		\
	$(subscriber_VALASOURCES:.vala=.h)		\
	$(NULL)

subscriber_SOURCES = \
	$(subscriber_GENERATEDSOURCES)		\
	libcontextprovider/intset.c					\
	libcontextprovider/intset.h					\
	$(NULL)

subscriber.vala.stamp: $(subscriber_VALASOURCES) $(subscriber_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@

subscriber_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
subscriber_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider.la

manager_VAPISOURCES = \
	libcontextprovider/intset.vapi				\
	$(NULL)

manager_VALASOURCES = \
	libcontextprovider/value_compare.vala				\
	libcontextprovider/manager.vala				\
	libcontextprovider/subscriber.vala				\
	libcontextprovider/context_provider.vala				\
	libcontextprovider/key_usage_counter.vala				\
	libcontextprovider/string_set.vala				\
	libcontextprovider/group.vala				\
	libcontextprovider/grouplist.vala				\
	libcontextprovider/dbus_interface.vala				\
	test_manager.vala			\
	$(NULL)

manager_GENERATEDSOURCES = \
	$(manager_VALASOURCES:.vala=.c)		\
	$(manager_VALASOURCES:.vala=.h)		\
	$(NULL)

manager_SOURCES = \
	$(manager_GENERATEDSOURCES)		\
	libcontextprovider/intset.c					\
	libcontextprovider/intset.h					\
	$(NULL)

manager.vala.stamp: $(manager_VALASOURCES) $(manager_VAPISOURCES)
	$(VALAC) -C --basedir $(top_srcdir) --vapidir=$(top_srcdir)/vapi --pkg gee-1.0 --pkg dbus-glib-1 $^
	touch $@

manager_CFLAGS = -I$(top_srcdir)/libcontextprovider $(CONTEXTD_CFLAGS) -DG_LOG_DOMAIN=\"ContextKit\"
manager_LDADD = $(CONTEXTD_LIBS) $(top_builddir)/libcontextprovider.la

EXTRA_DIST += \
	$(intset_VALASOURCES)				\
	$(intset_VAPISOURCES)				\
	$(intset_GENERATEDSOURCES)			\
	intset.vala.stamp				\
	$(stringset_VALASOURCES)			\
	$(stringset_VAPISOURCES)                       \
	$(stringset_GENERATEDSOURCES)			\
	stringset.vala.stamp			\
	$(value_compare_VALASOURCES)			\
	$(value_compare_SOURCES)		\
	value_compare.vala.stamp			\
	$(group_VALASOURCES)			\
	$(group_SOURCES)		\
	group.vala.stamp			\
	$(key_usage_counter_VALASOURCES)			\
	$(key_usage_counter_SOURCES)		\
	key_usage_counter.vala.stamp			\
	$(subscriber_VALASOURCES)			\
	$(subscriber_VAPISOURCES)			\
	$(subscriber_GENERATEDSOURCES)		\
	subscriber.vala.stamp			\
	$(manager_VALASOURCES)			\
	$(manager_VAPISOURCES)			\
	$(manager_GENERATEDSOURCES)		\
	manager.vala.stamp			\
	$(NULL)

DISTCLEANFILES += \
	$(NULL)

MAINTAINERCLEANFILES += \
	$(intset_GENERATEDSOURCES)			\
	intset.vala.stamp				\
	$(stringset_GENERATEDSOURCES)			\
	stringset.vala.stamp				\
	$(value_compare_SOURCES)			\
	value_compare.vala.stamp			\
	$(group_SOURCES)			\
	group.vala.stamp			\
	$(grouplist_SOURCES)			\
	grouplist.vala.stamp			\
	$(key_usage_counter_SOURCES)			\
	key_usage_counter.vala.stamp			\
	$(subscriber_GENERATEDSOURCES)			\
	subscriber.vala.stamp			\
	$(manager_GENERATEDSOURCES)			\
	manager.vala.stamp			\
	$(NULL)
