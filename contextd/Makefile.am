SUBDIRS = src doc unit-tests

check-sum:
	$(MAKE) -C unit-tests check-sum

coverage: covoptioncheck
	$(MAKE) -C unit-tests coverage

check-customer:
	$(MAKE) -C customer-tests check-customer

.PHONY: coverage check-sum

include $(top_srcdir)/am/covoptioncheck.am
