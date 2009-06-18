dnl Usage: AC_DISABLE_DOC_IF_EMPTY([variable])
AC_DEFUN([AC_DISABLE_DOC_IF_COLON], [
if test x$$1 = x: ; then
  AC_MSG_WARN([$1 not found, not building documentation])
  build_docs=no
fi])
