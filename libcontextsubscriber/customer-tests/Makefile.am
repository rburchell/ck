SUBDIRS = update-contextkit-providers testplugins
CHECKSUBDIRS = update-contextkit-providers

libcontextsubscribertestsdir = $(datadir)/libcontextsubscriber-tests
libcontextsubscribertests_DATA = tests.xml

check-customer:
	$(MAKE) -C $(top_srcdir) all
	./runTests.sh
	for i in $(CHECKSUBDIRS); do	$(MAKE) -C $$i $@; done

CLEANFILES = *.pyc

EXTRA_DIST = asynchronicity commander subscription runTests.sh	\
			 tests.xml common registry pluginchanging
