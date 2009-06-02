all: doxygen-maint

DOXYCFG = $(srcdir)/doxy.cfg

doxygen-maint:
	if test "$(DOXYGEN)" = doxygen; then \
		$(MAKE) $(AM_MAKEFLAGS) doxygen; \
	fi

doxygen:
	srcdir=$(srcdir) doxygen $(DOXYCFG)

EXTRA_DIST = $(DOXYCFG) html

distclean-local:
	rm -f doxygen.log

maintainer-clean-local:
	-rm -rf html

.PHONY: doxygen doxygen-maint
