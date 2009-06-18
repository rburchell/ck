SUBDIRS = update-contextkit-providers

libcontextsubscribertestsdir = $(datadir)/libcontextsubscriber-tests
libcontextsubscribertests_DATA = tests.xml

check-accept:
	$(MAKE) -C $(top_srcdir) all
	./runTests.sh
	for i in $(SUBDIRS); do	$(MAKE) -C $$i $@; done

CLEANFILES = *.pyc

EXTRA_DIST = asynchronicity commander subscription runTests.sh \
			 common
