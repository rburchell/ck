SUBDIRS = src unit-tests accept-tests cli update-contextkit-providers doc man

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = contextsubscriber-1.0.pc

check-sum:
	make -C unit-tests/ check-sum

check-accept:
	$(MAKE) -C accept-tests check-accept

covoptioncheck:
	@if test "$(GCOV)" = "" ; then \
		echo "Didn't configured for coverage, please rerun configure with --with-coverage"; \
		exit 1; \
	fi

coverage: covoptioncheck
	make -C unit-tests/ coverage

.PHONY: covoptioncheck coverage check-sum
