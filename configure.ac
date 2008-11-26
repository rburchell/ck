AC_INIT([ContextKit], [0.1.0], [rob.taylor@codethink.co.uk], ContextKit)
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])
AM_MAINTAINER_MODE

# Honor aclocal flags
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"
AC_CONFIG_MACRO_DIR(m4)

AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

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

GLIB_REQUIRED=2.12.0
GEE_REQUIRED=0.1.3

PKG_CHECK_MODULES(CONTEXTD, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED sqlite3 dbus-glib-1 gee-1.0 >=  $GEE_REQUIRED)
AC_SUBST(CONTEXTD_CFLAGS)
AC_SUBST(CONTEXTD_LIBS)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
