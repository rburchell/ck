SUBDIRSTESTS = \
	provider 							\
	propertyhandle							\
	handlesignalrouter               				\
	cdb 								\
	infoxmlkeysfinder						\
	infoxmlbackend 							\
	infocdbbackend 							\
	contextregistryinfo               				\
        contextpropertyinfo						\
	infobackend

SUBDIRS = $(SUBDIRSTESTS) util

check-sum: all
	@list='$(SUBDIRSTESTS)'; for subdir in $$list; do \
          (cd $$subdir && tput bold && echo $$subdir:\  && tput sgr0 && ( $(MAKE) $(AM_MAKEFLAGS) check 2>/dev/null | grep '^Total' | grep -v '^make' ) ) \
        done

coverage: covoptioncheck
	@list='$(SUBDIRSTESTS)'; for subdir in $$list; do \
          (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) coverage) \
        done
	mkdir -p coverage
	lcov $(patsubst %, -a %/coverage/selected.cov, $(SUBDIRSTESTS)) -o coverage/selecteds.cov
	@echo "real sources: `cd $(abs_top_srcdir) && pwd`"
	@echo "build: $(abs_top_builddir)"
	cat coverage/selecteds.cov | \
		sed "s,`cd $(abs_top_builddir) && pwd`,`cd $(abs_top_srcdir) && pwd`," | \
		sed 's,unit-tests/.*mock_,src/,' | \
		sed 's,unit-tests/.*src_,src/,' >coverage/sedded.cov
	genhtml --prefix `cd $(abs_top_srcdir) && pwd` -o coverage/ coverage/sedded.cov

clean-local:
	rm -rf coverage

.PHONY: coverage check-sum

include $(top_srcdir)/am/covoptioncheck.am
