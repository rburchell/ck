SUBDIRS = update-contextkit-providers

check-accept:
	$(MAKE) -C @abs_top_srcdir@ all
	./runTests.sh
	for i in $(SUBDIRS); do	$(MAKE) -C $$i $@; done

CLEANFILES = *.pyc

