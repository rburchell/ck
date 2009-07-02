SUBDIRS = src unit-tests customer-tests multithreading-tests cli update-contextkit-providers doc man

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextsubscriber-1.0.pc

check-sum:
	$(MAKE) -C unit-tests check-sum

check-customer:
	$(MAKE) -C customer-tests check-customer

coverage: covoptioncheck
	$(MAKE) -C unit-tests coverage

.PHONY: coverage check-sum

include $(top_srcdir)/am/covoptioncheck.am
