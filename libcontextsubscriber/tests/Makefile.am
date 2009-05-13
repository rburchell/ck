SUBDIRS = testclass xml-introspection-static xml-introspection-dynamic cdb cdb-introspection-static

check-sum: all
	@list='$(SUBDIRS)'; for subdir in $$list; do \
          if test "$$subdir" = "."; then \
            dot_seen=yes; \
            local_target="$$target-am"; \
          else \
            local_target="$$target"; \
          fi; \
          (cd $$subdir && tput bold && echo $$subdir:\  && tput sgr0 && ( $(MAKE) $(AM_MAKEFLAGS) check | grep '^Total' | grep -v '^make' ) ) \
          || eval $$failcom; \
        done

covoptioncheck:
	@if test "$(GCOV)" = "" ; then \
		echo "Didn't configured for coverage, please rerun configure with --with-coverage"; \
		exit 1; \
	fi

coverage: covoptioncheck
	@list='$(SUBDIRS)'; for subdir in $$list; do \
          if test "$$subdir" = "."; then \
            dot_seen=yes; \
            local_target="$$target-am"; \
          else \
            local_target="$$target"; \
          fi; \
          (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) coverage) || eval $$failcom; \
        done
	mkdir -p coverage
	lcov $(patsubst %, -a %/coverage/selected.cov, $(SUBDIRS)) -o coverage/selecteds.cov
	@echo "real sources: `cd $(abs_top_srcdir) && pwd`"
	@echo "build: $(abs_top_builddir)"
	cat coverage/selecteds.cov | \
		sed "s,`cd $(abs_top_builddir) && pwd`,`cd $(abs_top_srcdir) && pwd`," | \
		sed 's,tests/.*mock_,src/,' >coverage/sedded.cov
	genhtml --prefix `cd $(abs_top_srcdir) && pwd` -o coverage/ coverage/sedded.cov

clean-local:
	rm -rf coverage

.PHONY: covoptioncheck coverage check-sum
