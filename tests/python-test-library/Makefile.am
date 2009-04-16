DBUS_TESTS = contextProviderTCs.py

TESTS_ENVIRONMENT = \
        PYTHONPATH=@abs_top_srcdir@/tests/dbus:@abs_top_builddir@/tests/python-test-library

check-local: check-dbus

CHECK_DBUS_SLEEP=0

check-dbus:
        $(MAKE) -C testfw
        rm -f testfw/core
        rm -f testfw/testing.log
        TMPSYSFS=`mktemp -d`; \
        CONTEXT_EXAMPLE_SYSFS_LOCATION=$$TMPSYSFS; \
        export CONTEXT_EXAMPLE_SYSFS_LOCATION; \
        sleep=$(CHECK_DBUS_SLEEP); \
        echo "CONTEXT_EXAMPLE_SYSFS_LOCATION: $$CONTEXT_EXAMPLE_SYSFS_LOCATION"; \
        sh $(srcdir)/testfw/with-session-bus.sh \
                --sleep=$$sleep \
                --config-file=tools/tmp-session-bus.conf \
                --sleep=$$sleep \
                --config-file=tools/tmp-session-bus.conf \
                -- $(MAKE) check-TESTS \
                TESTS="$(DBUS_TESTS)" \
                TESTS_ENVIRONMENT="CONTEXT_EXAMPLE_SYSFS_LOCATION=$$TMPSYSFS $(TESTS_ENVIRONMENT) $(TEST_PYTHON)"; \
        rm -rf $$TMPSYSFS;
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
        testhelpers.py

CLEANFILES = *.pyc */*.pyc config.py

check_misc_sources = $(TESTS)

SUBDIRS = testfw
