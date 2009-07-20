SUBDIRS = src test-cli unit-tests doc customer-tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc

check-sum:
	$(MAKE) -C unit-tests check-sum

coverage: covoptioncheck
	$(MAKE) -C unit-tests coverage

check-customer:
	$(MAKE) -C customer-tests check-customer

.PHONY: coverage check-sum

include $(top_srcdir)/am/covoptioncheck.am
