#SUBDIRS = src unit_tests doc
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src unit_tests cli update-tool doc man

check-sum:
	make -C unit_tests/ check-sum

covoptioncheck:
	@if test "$(GCOV)" = "" ; then \
		echo "Didn't configured for coverage, please rerun configure with --with-coverage"; \
		exit 1; \
	fi

coverage: covoptioncheck
	make -C unit_tests/ coverage

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
