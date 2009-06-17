AC_PREREQ([2.61])
AC_INIT([ContextKit], [0.2.7~unreleased], [marius.vollmer@nokia.com], ContextKit)

AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2 tar-ustar 1.9])
#AM_INIT_AUTOMAKE([-Wall -Wno-portability -Wno-override -Werror dist-bzip2 tar-ustar 1.9])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AM_PATH_PYTHON

# always build with warnings
CXXFLAGS="$CXXFLAGS -Wall"
CFLAGS="$CXXFLAGS -Wall"

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

AC_CHECK_LIB([cdb], [cdb_make_start], [], [AC_MSG_ERROR([Can't find cdb library])])

# setup cflags and libs for different parts of the code
# fixme: why here?  this belongs to the internals of that part of the code -> Makefile.am
CONTEXTPROVIDER_CFLAGS="$GLIB_CFLAGS $GOBJECT_CFLAGS $GDBUS_CFLAGS $GEE_CFLAGS"
CONTEXTPROVIDER_LIBS="$GLIB_LIBS $GOBJECT_LIBS $GDBUS_LIBS $GEE_LIBS"

CONTEXTD_CFLAGS="$GLIB_CFLAGS $GOBJECT_CFLAGS $GDBUS_CFLAGS $GEE_CFLAGS $HAL_CFLAGS"
CONTEXTD_LIBS="$GLIB_LIBS $GOBJECT_LIBS $GDBUS_LIBS $GEE_LIBS $HAL_LIBS"

AC_SUBST(CONTEXTD_CFLAGS)
AC_SUBST(CONTEXTD_LIBS)

AC_SUBST(CONTEXTPROVIDER_CFLAGS)
AC_SUBST(CONTEXTPROVIDER_LIBS)

dnl have_location=no
dnl PKG_CHECK_MODULES(LOCATION, liblocation, [have_location=yes], [have_location=no])
dnl AC_SUBST(LOCATION_CFLAGS)
dnl AC_SUBST(LOCATION_LIBS)
dnl AM_CONDITIONAL(HAVE_LOCATION, test x$have_location = xyes)

# fixme: if not found, disable building of documentation
GTK_DOC_CHECK([1.9])

# todo?: add vala version check here
AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)

build_docs=yes

AC_SUBST([XSLTPROC], [])
AC_CHECK_PROGS([XSLTPROC], [xsltproc])
AC_DISABLE_DOC_IF_EMPTY([XSLTPROC])

AC_SUBST([XMLLINT], [])
AC_CHECK_PROGS([XMLLINT], [xmllint])
AC_DISABLE_DOC_IF_EMPTY([XMLLINT])

AC_PROG_PERL_MODULES([XML::LibXML::Reader], [], [build_docs=no])

# todo?: add asciidoc version check
# todo?: add program check for dot
# todo?: add program check for source-highlight

AM_CONDITIONAL(CONTEXTKIT_BUILD_DOCS, test x$build_docs = xyes)

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

DEFAULT_CONTEXT_PROVIDERS="$datadir/contextkit/providers/"
AC_SUBST(DEFAULT_CONTEXT_PROVIDERS)

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
        doc/reference/libcontextprovider/Makefile tools/Makefile
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
