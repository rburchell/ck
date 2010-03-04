SUBDIRS = src unit-tests doc customer-tests context-provide

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextprovider-1.0.pc
noinst_DATA = contextprovider-1.0-uninstalled.pc
dbusconfdir = $(sysconfdir)/dbus-1/system.d
dist_dbusconf_DATA = libcontextprovider0.conf

check-sum:
	$(MAKE) -C unit-tests check-sum

coverage: covoptioncheck
	$(MAKE) -C unit-tests coverage
	$(MAKE) -C customer-tests coverage
	mkdir -p coverage
	lcov -a unit-tests/coverage/sedded.cov -a customer-tests/coverage/src.cov -o coverage/all.cov
	genhtml -o coverage/ coverage/all.cov

check-customer:
	$(MAKE) -C customer-tests check-customer

.PHONY: coverage check-sum

include $(top_srcdir)/am/covoptioncheck.am
