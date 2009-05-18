AC_INIT([libcontextsubscriber], [0.1], [marius.vollmer@nokia.com])
AM_INIT_AUTOMAKE([-Wall foreign dist-bzip2])
AM_MAINTAINER_MODE

AC_SUBST([USE_MAINTAINER_MODE])
AC_CONFIG_MACRO_DIR([m4])

AS_IF([test "$USE_MAINTAINER_MODE" = yes],
	[withm_coverage=yes
	 withm_doxygen=yes])

AC_ARG_WITH([coverage],
	[AS_HELP_STRING([--with-coverage],
		[when running testcases, create coverage data by gcov and process by lcov])],
	[with_coverage=$withval])

AC_ARG_WITH([doxygen],
	[AS_HELP_STRING([--with-doxygen],
		[regenerate doxygen documentation by default in the doc subdir])],
	[with_doxygen=$withval])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
AM_PATH_PYTHON
AC_CHECK_PROGS([BC], [bc])
AS_IF([test "$with_coverage" = yes -o \( "$with_coverage" = "" -a "$withm_coverage" = yes \)],
	    [AC_CHECK_PROGS([GCOV], [gcov], [:])
	     AC_CHECK_PROGS([LCOV], [lcov], [:])
	     if test "$GCOV" = : -o "$LCOV" = :; then
	        AC_MSG_ERROR([For coverage you need lcov and gcov installed.  Probably --enable-maintainer-mode enabled coverage.])
	     fi
	     COV_CXXFLAGS="$COV_CXXFLAGS -ftest-coverage -fprofile-arcs -fno-elide-constructors"
	     COV_LIBS="$COV_LIBS -lgcov"
	     AC_SUBST([COV_CXXFLAGS])
	     AC_SUBST([COV_LIBS])
	     ],
	    [AC_MSG_NOTICE([No coverage requested, enable it with --with-coverage])])
AS_IF([test "$with_doxygen" = yes -o \( "$with_doxygen" = "" -a "$withm_doxygen" = yes \)],
	    [AC_CHECK_PROGS([DOXYGEN], [doxygen], [:])
	     if test "$DOXYGEN" = : ; then
	        AC_MSG_ERROR([No doxygen found, probably --enable-maintainer-mode enabled doxygen.])
	     fi
	     AC_SUBST([DOXYGEN])
	     ],
	    [AC_MSG_NOTICE([Doxygen not enabled, enable it with --with-doxygen])])

# Checks for libraries.
PKG_CHECK_MODULES([Qt], [QtCore QtGui QtDBus QtXml])
MOC=`pkg-config QtCore --variable=moc_location`
AC_CHECK_FILE([$MOC], [],
   AC_MSG_ERROR([Can't find moc compiler: $MOC]))
AC_SUBST([MOC])
UIC=`pkg-config QtCore --variable=uic_location`
AC_CHECK_FILE([$UIC], [],
   AC_MSG_ERROR([Can't find uic compiler: $UIC]))
AC_SUBST([UIC])
RCC=`pkg-config QtCore --variable=exec_prefix`/bin/rcc
AC_CHECK_FILE([$RCC], [],
   AC_MSG_ERROR([Can't find rcc compiler: $RCC]))
AC_SUBST([RCC])

CXXFLAGS="$CXXFLAGS $Qt_CFLAGS"
LIBS="$LIBS $Qt_LIBS"
PKG_CHECK_MODULES([QtTest], [QtTest])

#AC_CONFIG_HEADER([config.h])

AC_CONFIG_FILES([Makefile
		 src/Makefile
         src/infobackendconfig.h
		 cli/Makefile
         update-tool/Makefile
         update-tool/update-tool.h
		 unit-tests/Makefile
		 doc/Makefile
         man/Makefile
         unit-tests/xml-introspection-static/Makefile
         unit-tests/cdb-introspection-static/Makefile
         unit-tests/xml-introspection-dynamic/Makefile
         unit-tests/cdb-introspection-dynamic/Makefile
         unititests/contextregistryinfo-xml-static/Makefile
         unit-tests/cdb/Makefile
         unit-tests/cdbreader/Makefile
         unit-tests/cdbwriter/Makefile
	 unit-tests/propertyprovider/Makefile
	 unit-tests/propertyhandle/Makefile])

AC_OUTPUT

# Checks for header files.
# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
# Output files.
#AC_CONFIG_HEADERS([config.h])             # optional

# Full from ContextCommander -- coming soon
#AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile tests/Makefile
#			  tests/signallingmap/Makefile
#			  tests/applicationlist/Makefile
#			  tests/subscriber/Makefile
#			  tests/boolvariant/Makefile
#			  tests/commandmodel/Makefile
#			  tests/manager/Makefile
#			  tests/inspectmodel/Makefile
#			  debian-devel/control debian-devel/rules])


