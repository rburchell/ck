ACLOCAL_AMFLAGS = -I m4
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

EXTRA_DIST = accept-tests

#deb:
#	rm -rf debian/
#	mkdir debian/
#	cp -a debian-devel/rules debian/
#	chmod a+x debian/rules
#	cp -a debian-devel/control debian/
#	EDITOR=/bin/true dch --create --package contextcommander-devel --newversion 0~devel --distribution unstable --force-distribution
#	fakeroot debian/rules binary
#	rm -rf debian/

#debinst: deb
#	sudo dpkg -i ../@PACKAGE@-devel_0~devel_`dpkg-architecture | grep BUILD_ARCH= | cut -d= -f2`.deb

#.PHONY: covoptioncheck coverage check-sum deb debinst
.PHONY: covoptioncheck coverage check-sum
