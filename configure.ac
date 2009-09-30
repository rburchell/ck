AC_PREREQ([2.61])
AC_INIT([ContextKit], [0.3.7.1], [marius.vollmer@nokia.com], ContextKit)

AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2 tar-ustar 1.9])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
CFLAGS="$CXXFLAGS -Wall"
AC_PROG_CXX
CXXFLAGS="$CXXFLAGS -Wall"
AC_PROG_LIBTOOL
AM_PATH_PYTHON

# check for libraries
PKG_CHECK_MODULES_SUBST([QtTest], [QtTest])
PKG_CHECK_MODULES_SUBST([QtCore], [QtCore])
PKG_CHECK_MODULES_SUBST([QtDBus], [QtDBus])
PKG_CHECK_MODULES_SUBST([QtXml], [QtXml])
# only for compiling c code
PKG_CHECK_MODULES_SUBST([DBUS], [dbus-1])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.12.0])

AC_PROG_QT_MOC
AC_PROG_QT_UIC
AC_PROG_QT_RCC
AC_CHECK_LIB([cdb], [cdb_make_start], [CDB_LIBS=-lcdb], [AC_MSG_ERROR([Can't find cdb library])])
AC_SUBST([CDB_LIBS])

# tools for documentation
AX_FEATURE_DISABLEABLE([doc], [BUILD_DOCS], [disable building of documentation])
AC_MSG_CHECKING([for gtk-doc])
PKG_CHECK_EXISTS([gtk-doc >= 1.9],[AC_MSG_RESULT([yes])],
			  [AC_MSG_RESULT([no])
			   missing_deps_BUILD_DOCS="$missing_deps_BUILD_DOCS, gtk-doc >= 1.9"])
HTML_DIR="${datadir}/gtk-doc/html"
AC_SUBST([HTML_DIR])
AX_DISABLE_FEATURE_ON_PROG([Dot drawing tool], [BUILD_DOCS], [DOT], [dot])
AX_DISABLE_FEATURE_ON_PROG([Asciidoc], [BUILD_DOCS], [ASCIIDOC], [asciidoc], [8.2.7])
AX_DISABLE_FEATURE_ON_PROG([source-highlight], [BUILD_DOCS], [SOURCE_HIGHLIGHT], [source-highlight])
AX_DISABLE_FEATURE_ON_PROG([xsltproc], [BUILD_DOCS], [XSLTPROC], [xsltproc])
AX_DISABLE_FEATURE_ON_PROG([xmllint], [BUILD_DOCS], [XMLLINT], [xmllint])
AX_DISABLE_FEATURE_ON_PROG([doxygen], [BUILD_DOCS], [DOXYGEN], [doxygen])
AM_CONDITIONAL([CONTEXTKIT_BUILD_DOCS], [test "$missing_deps_BUILD_DOCS" = ""])
AM_CONDITIONAL([GTK_DOC_USE_LIBTOOL], [test "$missing_deps_BUILD_DOCS" = ""])
AM_CONDITIONAL([ENABLE_GTK_DOC], [test "$missing_deps_BUILD_DOCS" = ""])

# coverage tools
AX_FEATURE_DISABLEABLE([coverage], [COVERAGE], [disable unittests' coverage support])
AX_DISABLE_FEATURE_ON_PROG([GCov coverage recording tool], [COVERAGE], [GCOV], [gcov])
AX_DISABLE_FEATURE_ON_PROG([LCov coverage visualization tool], [COVERAGE], [LCOV], [lcov])
AM_CONDITIONAL([CONTEXTKIT_COVERAGE], [test "$missing_deps_COVERAGE" = ""])

AC_CONFIG_FILES([
        Makefile
        doc/Makefile
	common/Makefile
        libcontextsubscriber/Makefile
        libcontextsubscriber/cli/Makefile
        libcontextsubscriber/reg-cli/Makefile
        libcontextsubscriber/contextsubscriber-1.0.pc
        libcontextsubscriber/customer-tests/Makefile
        libcontextsubscriber/customer-tests/update-contextkit-providers/Makefile
        libcontextsubscriber/customer-tests/testplugins/Makefile
        libcontextsubscriber/customer-tests/testplugins/timeplugin1/Makefile
        libcontextsubscriber/customer-tests/testplugins/timeplugin2/Makefile
        libcontextsubscriber/doc/Makefile
        libcontextsubscriber/man/Makefile
        libcontextsubscriber/src/Makefile
        libcontextsubscriber/unit-tests/Makefile
        libcontextsubscriber/unit-tests/cdb/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-cdb-dynamic/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-cdb-static/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-xml-dynamic/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-xml-static/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-cdb-dynamic/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-cdb-static/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-xml-dynamic/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-xml-static/Makefile
        libcontextsubscriber/unit-tests/handlesignalrouter/Makefile
        libcontextsubscriber/unit-tests/propertyhandle/Makefile
        libcontextsubscriber/unit-tests/provider/Makefile
        libcontextsubscriber/unit-tests/infoxmlkeysfinder/Makefile
        libcontextsubscriber/unit-tests/util/Makefile
        libcontextsubscriber/multithreading-tests/Makefile
        libcontextsubscriber/multithreading-tests/new-property-in-thread/Makefile
        libcontextsubscriber/multithreading-tests/old-property-in-thread/Makefile
        libcontextsubscriber/multithreading-tests/single-thread/Makefile
        libcontextsubscriber/multithreading-tests/stress-test/Makefile
        libcontextsubscriber/multithreading-tests/using-backend-from-thread/Makefile
        libcontextsubscriber/multithreading-tests/wait-for-subscription-only-in-thread/Makefile
        libcontextsubscriber/multithreading-tests/wait-for-subscription-thread/Makefile
        libcontextsubscriber/update-contextkit-providers/Makefile
	libcontextprovider/contextprovider-1.0.pc
	libcontextprovider/Makefile
	libcontextprovider/doc/Makefile
	libcontextprovider/src/Makefile
	libcontextprovider/unit-tests/Makefile
	libcontextprovider/unit-tests/manager/Makefile
	libcontextprovider/unit-tests/subscriber/Makefile
	libcontextprovider/unit-tests/contextgroup/Makefile
	libcontextprovider/unit-tests/property/Makefile
	libcontextprovider/unit-tests/contextc/Makefile
	libcontextprovider/unit-tests/service/Makefile
	libcontextprovider/unit-tests/servicebackend/Makefile
	libcontextprovider/customer-tests/Makefile
	libcontextprovider/customer-tests/subscription/Makefile
	libcontextprovider/customer-tests/client/Makefile
	libcontextprovider/customer-tests/value-changes/Makefile
	libcontextprovider/customer-tests/types/Makefile
	libcontextprovider/customer-tests/c-api/Makefile
        python/Makefile
        spec/Makefile
        tools/Makefile
        tools/loadmem/Makefile
])

AC_OUTPUT

AX_FEATURE_SUMMARIZE([BUILD_DOCS],
		     [Building of documentation is enabled],
		     [Documentation won't be built])

AX_FEATURE_SUMMARIZE([COVERAGE],
		     [Unit testing coverage support is enabled],
		     [Coverage won't be supported during unit testing])
