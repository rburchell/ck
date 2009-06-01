DBUS_TESTS = $(srcdir)/contextProviderTCs.py

TESTS =

TESTS_ENVIRONMENT = \
		PYTHONPATH=@abs_top_srcdir@/tests/dbus:@abs_top_builddir@/tests/python-test-library:@abs_top_srcdir@/tests/python-test-library

check-local: check-dbus

CHECK_DBUS_SLEEP=0

check-dbus:
		$(MAKE) -C testfw
		rm -f testfw/core
		rm -f testfw/testing.log
		TMPSYSFS=`mktemp -d` \
		$(srcdir)/testfw/with-session-bus.sh \
				--sleep=$(CHECK_DBUS_SLEEP) \
				--config-file=testfw/tmp-session-bus.conf \
				-- $(MAKE) check-TESTS \
				TESTS="$(DBUS_TESTS)" \
				TESTS_ENVIRONMENT="CONTEXT_EXAMPLE_SYSFS_LOCATION=$$TMPSYSFS $(TESTS_ENVIRONMENT) $(TEST_PYTHON)"; e=$$?; \
		rm -rf $$TMPSYSFS; \
		exit $$e
		@if test -e testfw/core; then\
			echo "Core dump exists: testfw/core";\
			exit 1;\
		fi

config.py: Makefile
		echo "PACKAGE_STRING = \"$(PACKAGE_STRING)\"" > config.py

BUILT_SOURCES = config.py

EXTRA_DIST = \
		$(DBUS_TESTS) \
		servicetest.py \
		contexttest.py \
		inotify.py \
		sysfsemulator.py \
		eventprotocol.py \
		testhelpers.py \
		contextProviderTCs.py \
		stubs/provider_stub.py \
		conf.py

CLEANFILES = *.pyc */*.pyc config.py

check_misc_sources = $(TESTS)

SUBDIRS = testfw
