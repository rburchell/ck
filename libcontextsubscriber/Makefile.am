SUBDIRS = src unit-tests accept-tests cli update-contextkit-providers doc man

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextsubscriber-1.0.pc

check-sum:
	$(MAKE) -C unit-tests check-sum

check-accept:
	$(MAKE) -C accept-tests check-accept

coverage: covoptioncheck
	$(MAKE) -C unit-tests coverage

.PHONY: coverage check-sum

include $(top_srcdir)/am/covoptioncheck.am
