SUBDIRS = src unit-tests customer-tests cli cls update-contextkit-providers doc man propertymonitor

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextsubscriber-1.0.pc
noinst_DATA = contextsubscriber-1.0-uninstalled.pc

check-sum:
	$(MAKE) -C unit-tests check-sum

check-customer:
	$(MAKE) -C customer-tests check-customer

coverage: covoptioncheck
	$(MAKE) -C unit-tests coverage
	$(MAKE) -C customer-tests coverage

.PHONY: coverage check-sum

include $(top_srcdir)/am/covoptioncheck.am
