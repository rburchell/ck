all: doxygen-maint

DOXYCFG = $(srcdir)/doxy.cfg

doxygen-maint:
	if test "$(DOXYGEN)" = doxygen; then \
		$(MAKE) $(AM_MAKEFLAGS) doxygen; \
	fi

doxygen:
	srcdir=$(srcdir) doxygen $(DOXYCFG)

EXTRA_DIST = $(DOXYCFG) html

maintainer-clean-local:
	-rm -rf html doxygen.log

.PHONY: doxygen
