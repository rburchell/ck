AC_PREREQ([2.61])
AC_INIT([ContextKit], [0.2.6], [marius.vollmer@nokia.com], ContextKit)

AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([-Wall -Wno-portability -Wno-override -Werror dist-bzip2 tar-ustar 1.9])

# Honor aclocal flags
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"
AC_CONFIG_MACRO_DIR(m4)

AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC

AC_PROG_LIBTOOL

GTK_DOC_CHECK([1.9])

AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)

build_docs=yes

XSLTPROC=
AC_CHECK_PROGS([XSLTPROC], [xsltproc])
if test -z "$XSLTPROC"; then
  AC_MSG_WARN([xsltproc (from the libxslt source package) not found, not building documentation])
  build_docs=no
fi

XMLLINT=
AC_CHECK_PROGS([XMLLINT], [xmllint])
if test -z "$XMLLINT"; then
  AC_MSG_WARN([xmllint (from the libxml2-utils source package) not found, not building documentation])
  build_docs=no
fi

AC_PROG_PERL_MODULES([XML::LibXML::Reader], [],
[
  build_docs=no
])

AM_CONDITIONAL(CONTEXTKIT_BUILD_DOCS, test x$build_docs = xyes)

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

######################
# Coverage option
######################

AC_ARG_ENABLE(coverage,
	AS_HELP_STRING([--enable-coverage=@<:@no/yes/auto@:>@],
	[compile with coverage profiling and debug (gcc only)]),
	enable_coverage=$enableval,enable_coverage=no)

######################
# Debug option
######################

AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug=@<:@no/yes/auto@:>@],
	[compile with debug profiling]),
	enable_debug=$enableval,enable_debug=no)

GLIB_REQUIRED=2.12.0
GEE_REQUIRED=0.1.3

PKG_CHECK_MODULES(GLIBCOMMON, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED dbus-glib-1)
PKG_CHECK_MODULES(GEE, gee-1.0 >= $GEE_REQUIRED)
PKG_CHECK_MODULES(HAL, hal)

CONTEXTD_CFLAGS="$GLIBCOMMON_CFLAGS $GEE_CFLAGS $HAL_CFLAGS"
CONTEXTD_LIBS="$GLIBCOMMON_LIBS $GEE_LIBS $HAL_LIBS"

CONTEXTPROVIDER_CFLAGS="$GLIBCOMMON_CFLAGS $GEE_CFLAGS"
CONTEXTPROVIDER_LIBS="$GLIBCOMMON_LIBS $GEE_LIBS"


##################################
# Flags for coverage measurement
##################################

if test "x$enable_coverage" != "xno"; then   	
	CONTEXTD_CFLAGS="$CONTEXTD_CFLAGS -fprofile-arcs -ftest-coverage"
	CONTEXTD_LIBS="$CONTEXTD_LIBS -lgcov"
	#FIXME: preserve non -Wall/-g/-O flags from users CFLAGS
	CFLAGS="-Wall -g -O0"
	AC_CHECK_PROGS([HAVE_LCOV], [lcov],)
	if test -z "$HAVE_LCOV"; then
	  AC_MSG_ERROR([lcov is necessary for testing code coverage (http://ltp.sourceforge.net/coverage/lcov.php)])
	fi
fi

AM_CONDITIONAL(HAVE_COVERAGE, test "x$enable_coverage" != "xno")

##################################
# Flags for debugging
##################################

if test "x$enable_debug" != "xno" ; then
   	CONTEXTD_CFLAGS="$CONTEXTD_CFLAGS -g -Wall -Werror"
fi 

AC_SUBST(CONTEXTD_CFLAGS)
AC_SUBST(CONTEXTD_LIBS)

AC_SUBST(CONTEXTPROVIDER_CFLAGS)
AC_SUBST(CONTEXTPROVIDER_LIBS)

have_location=no
PKG_CHECK_MODULES(LOCATION, liblocation, [have_location=yes], [have_location=no])
AC_SUBST(LOCATION_CFLAGS)
AC_SUBST(LOCATION_LIBS)

AM_CONDITIONAL(HAVE_LOCATION, test x$have_location = xyes)

AC_CONFIG_SUBDIRS([libcontextsubscriber])
AM_PATH_PYTHON(,, [:])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libcontextprovider/Makefile])
AC_CONFIG_FILES([contextd/Makefile])
AC_CONFIG_FILES([spec/Makefile])
AC_CONFIG_FILES([tests/Makefile])
AC_CONFIG_FILES([tests/unit_tests/Makefile])
AC_CONFIG_FILES([tests/contextd_tests/Makefile])
AC_CONFIG_FILES([tests/python-test-library/Makefile])
AC_CONFIG_FILES([tests/python-test-library/testfw/Makefile])
AC_CONFIG_FILES([tests/accept-tests/Makefile])
AC_CONFIG_FILES([doc/Makefile])
AC_CONFIG_FILES([doc/reference/Makefile])
AC_CONFIG_FILES([doc/reference/libcontextprovider/Makefile])
AC_CONFIG_FILES([tools/Makefile])
AC_CONFIG_FILES([python/Makefile])
AC_CONFIG_FILES([contextprovider-1.0.pc])
AC_CONFIG_FILES([contextd/org.freedesktop.ContextKit.contextd.service])
AC_CONFIG_FILES([fake-provider/Makefile])
AC_CONFIG_FILES([vapi/Makefile])

AC_OUTPUT

