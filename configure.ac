AC_PREREQ([2.61])
AC_INIT([ContextKit], [0.2.7~unreleased], [marius.vollmer@nokia.com], ContextKit)

AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2 tar-ustar 1.9])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
CFLAGS="$CXXFLAGS -Wall"
AC_PROG_CXX
CXXFLAGS="$CXXFLAGS -Wall"
AC_PROG_LIBTOOL
AM_PATH_PYTHON

# vala compiler (>= 0.7.1)
AC_FEATURE_DISABLEABLE([vala], [BUILD_VALA], [disable building of vala sources])
AC_DISABLE_FEATURE_ON_PROG([Vala compiler], [BUILD_VALA], [VALAC], [valac], [0.7.1])
AM_CONDITIONAL(CONTEXTKIT_BUILD_VALA, test "$missing_deps_BUILD_VALA" = "")

# check for libraries
PKG_CHECK_MODULES_SUBST([GLIB], [glib-2.0 >= 2.12.0])
PKG_CHECK_MODULES_SUBST([GOBJECT], [gobject-2.0 >= 2.12.0])
PKG_CHECK_MODULES_SUBST([GDBUS], [dbus-glib-1])
PKG_CHECK_MODULES_SUBST([GEE], [gee-1.0 >= 0.1.3])
PKG_CHECK_MODULES_SUBST([HAL], [hal])
PKG_CHECK_MODULES_SUBST([QtTest], [QtTest])
PKG_CHECK_MODULES_SUBST([QtCore], [QtCore])
PKG_CHECK_MODULES_SUBST([QtDBus], [QtDBus])
PKG_CHECK_MODULES_SUBST([QtXml], [QtXml])
AC_PROG_QT_MOC
AC_PROG_QT_UIC
AC_PROG_QT_RCC
AC_CHECK_LIB([cdb], [cdb_make_start], [CDB_LIBS=-lcdb], [AC_MSG_ERROR([Can't find cdb library])])
AC_SUBST([CDB_LIBS])

# tools for documentation
AC_FEATURE_DISABLEABLE([doc], [BUILD_DOCS], [disable building of documentation])
AC_MSG_CHECKING([for gtk-doc])
PKG_CHECK_EXISTS([gtk-doc >= 1.9],[AC_MSG_RESULT([yes])],
			  [AC_MSG_RESULT([no])
			   missing_deps_BUILD_DOCS="$missing_deps_BUILD_DOCS, gtk-doc >= 1.9"])
HTML_DIR="${datadir}/gtk-doc/html"
AC_SUBST([HTML_DIR])
AC_DISABLE_FEATURE_ON_PROG([Dot drawing tool], [BUILD_DOCS], [DOT], [dot])
AC_DISABLE_FEATURE_ON_PROG([Asciidoc], [BUILD_DOCS], [ASCIIDOC], [asciidoc], [8.2.7])
AC_DISABLE_FEATURE_ON_PROG([source-highlight], [BUILD_DOCS], [SOURCE_HIGHLIGHT], [source-highlight])
AC_DISABLE_FEATURE_ON_PROG([xsltproc], [BUILD_DOCS], [XSLTPROC], [xsltproc])
AC_DISABLE_FEATURE_ON_PROG([xmllint], [BUILD_DOCS], [XMLLINT], [xmllint])
AC_DISABLE_FEATURE_ON_PROG([doxygen], [BUILD_DOCS], [DOXYGEN], [doxygen])
AM_CONDITIONAL(CONTEXTKIT_BUILD_DOCS, test "$missing_deps_BUILD_DOCS" = "")
AM_CONDITIONAL(GTK_DOC_USE_LIBTOOL, test "$missing_deps_BUILD_DOCS" = "")
AM_CONDITIONAL(ENABLE_GTK_DOC, test "$missing_deps_BUILD_DOCS" = "")

######################
# Coverage option
######################

dnl AC_ARG_ENABLE(coverage,
dnl     AS_HELP_STRING([--enable-coverage=@<:@no/yes/auto@:>@],
dnl     [compile with coverage profiling and debug (gcc only)]),
dnl     enable_coverage=$enableval,enable_coverage=no)

##################################
# Flags for coverage measurement
##################################

dnl if test "x$enable_coverage" != "xno"; then          
dnl     CONTEXTD_CFLAGS="$CONTEXTD_CFLAGS -fprofile-arcs -ftest-coverage"
dnl     CONTEXTD_LIBS="$CONTEXTD_LIBS -lgcov"
dnl     #FIXME: preserve non -Wall/-g/-O flags from users CFLAGS
dnl     CFLAGS="-Wall -g -O0"
dnl     AC_CHECK_PROGS([HAVE_LCOV], [lcov],)
dnl     if test -z "$HAVE_LCOV"; then
dnl       AC_MSG_ERROR([lcov is necessary for testing code coverage (http://ltp.sourceforge.net/coverage/lcov.php)])
dnl     fi
dnl fi

dnl enable_coverage=no
dnl AM_CONDITIONAL(HAVE_COVERAGE, test "x$enable_coverage" != "xno")

AC_CONFIG_FILES([
	Makefile
        contextd/Makefile
        spec/Makefile
        tests/Makefile
        tests/unit_tests/Makefile
        tests/contextd_tests/Makefile
        tests/python-test-library/Makefile
        tests/python-test-library/testfw/Makefile
        tests/accept-tests/Makefile doc/Makefile
        doc/reference/Makefile
        doc/reference/libcontextprovider/Makefile
        python/Makefile
        contextprovider-1.0.pc
        contextd/org.freedesktop.ContextKit.contextd.service
        fake-provider/Makefile
        vapi/Makefile
        libcontextprovider/Makefile
	libcontextsubscriber/Makefile
        libcontextsubscriber/src/Makefile
        libcontextsubscriber/cli/Makefile
        libcontextsubscriber/update-contextkit-providers/Makefile
        libcontextsubscriber/unit-tests/Makefile
        libcontextsubscriber/doc/Makefile
        libcontextsubscriber/man/Makefile
        libcontextsubscriber/contextsubscriber-1.0.pc
        libcontextsubscriber/unit-tests/util/Makefile
        libcontextsubscriber/unit-tests/cdbreader/Makefile
        libcontextsubscriber/unit-tests/cdbwriter/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-xml-static/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-xml-static/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-xml-dynamic/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-xml-dynamic/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-cdb-static/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-cdb-static/Makefile
        libcontextsubscriber/unit-tests/contextregistryinfo-cdb-dynamic/Makefile
        libcontextsubscriber/unit-tests/contextpropertyinfo-cdb-dynamic/Makefile
        libcontextsubscriber/unit-tests/propertyprovider/Makefile
        libcontextsubscriber/unit-tests/propertyhandle/Makefile
        libcontextsubscriber/unit-tests/handlesignalrouter/Makefile
        libcontextsubscriber/accept-tests/Makefile
        libcontextsubscriber/accept-tests/update-contextkit-providers/Makefile
])

AC_OUTPUT

AC_FEATURE_SUMMARIZE([BUILD_DOCS],
                     [Building of documentation is enabled],
                     [Documentation won't be built])

AC_FEATURE_SUMMARIZE([BUILD_VALA],
                     [Building of vala sources is enabled],
                     [Vala sources won't be built])
