dnl Usage: AC_DISABLE_DOC_IF_EMPTY([variable])
AC_DEFUN([AC_DISABLE_DOC_IF_COLON],
	[if test x$$1 = x: ; then
	  AC_MSG_WARN([$1 not found, not building documentation])
	  build_docs=no
	 fi])

AC_DEFUN([AC_CHECK_DOCPROG], [AC_CHECK_PROG([$1], [$2], [:])
			      AC_DISABLE_DOC_IF_COLON([$1])
			      AC_SUBST([$1])])

AC_DEFUN([AX_PROGRAM_VERSION], [dnl
`$$1 --version </dev/null 2>/dev/null | head -n1 | grep -o '[[0-9.]]*'`dnl
])

dnl Usage: AC_DISABLE_FEATURE_ON_PROG([$1=description], [$2=conditional feature],
dnl                                   [$3=subst var], [$4=lookup list],
dnl				      [$5=needed version (optional)])
AC_DEFUN([AC_DISABLE_FEATURE_ON_PROG], [
AC_ARG_VAR([$3], [$1])
AC_CHECK_PROGS([$3], [$4], [:])
if test $$3 = :
then
  missing_deps_$2="$missing_deps_$2, $1"
else
  if test x$5 != x
  then
    disable_feature_on_prog_ver=AX_PROGRAM_VERSION([$3])
    AX_COMPARE_VERSION([$disable_feature_on_prog_ver], [ge], [$5], [],
    		       [AC_MSG_WARN([$1 version mismatch; wanted $5, has: $disable_feature_on_prog_ver])
		        missing_deps_$2="$missing_deps_$2, $1 >= $5"])
  fi
fi
])

dnl Usage: AC_FEATURE_DISABLEABLE([$1=option name], [$2=conditional feature], [$3=description])
AC_DEFUN([AC_FEATURE_DISABLEABLE], [
AC_ARG_ENABLE([$1],
     AS_HELP_STRING([--disable-$1],
                    [$3 [[default=no]]]),,
     [enable_$1=undef])
if test x$enable_$1 = xno
then
	missing_deps_$2=", disabled by user"
fi
])

dnl Usage: AC_FEATURE_SUMMARIZE([$1=conditional feature], [$2=enabled msg], [$3=disabled msg])
AC_DEFUN([AC_FEATURE_SUMMARIZE], [
if test "$missing_deps_$1" = ""
then
	AC_MSG_NOTICE([$2])
else
	missing_deps_$1=`echo $missing_deps_$1 | cut -c3-`
	AC_MSG_WARN([$3, missing dependencies: $missing_deps_$1])
fi
])
