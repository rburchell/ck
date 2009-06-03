SUBDIRS = update-contextkit-providers

check-accept:
	$(MAKE) -C $(top_srcdir) all
	./runTests.sh
	for i in $(SUBDIRS); do	$(MAKE) -C $$i $@; done

CLEANFILES = *.pyc

EXTRA_DIST = asynchronocity.py commander_appearing.py	\
	commander_disabled.py commander_nonexistent.py	\
	FT_contextSubscriber.py runTests.sh rec-kill.sh
