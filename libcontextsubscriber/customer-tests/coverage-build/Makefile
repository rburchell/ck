# The basic idea is that you are using CCache for compiling, so we are
# happily cleaning your original build, because it can be rebuilt in 2
# seconds, but coverage build is slow, so we only build coverage for
# customer tests, not always.

# If you want to build a libcontextsubscriber with coverage support,
# you have to run make in this directory, the output is in
# .libs/libcontextsubscriber.so.0.0.0

all: clean-orig Makefile.coverage
	$(MAKE) -C ../../../common   # redo it now, so it won't be rebuilt with coverage
	$(MAKE) -f Makefile.coverage VPATH=../../src CXXFLAGS=--coverage

Makefile.coverage: ../../src/Makefile clean-orig
	cat $< | sed 's,^srcdir =.*,srcdir = ../../src, ; s,^top_builddir =.*,top_builddir = ../../.., ; s,^top_srcdir =.*,top_srcdir = ../../..,' >$@
	rm -rf .deps
	cp -av ../../src/.deps .

clean: clean-orig
	-$(MAKE) -f Makefile.coverage clean
	rm -f *.gcno
	rm -rf .deps
	rm -rf .libs
	rm -f Makefile.coverage

clean-orig:
	$(MAKE) -C ../../src clean

.PHONY: all clean clean-orig