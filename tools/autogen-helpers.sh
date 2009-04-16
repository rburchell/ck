# Not all echo versions allow -n, so we check what is possible. This test is
# based on the one in autoconf.
ECHO_C=
ECHO_N=
case `echo -n x` in
-n*)
  case `echo 'x\c'` in
  *c*) ;;
  *)   ECHO_C='\c';;
  esac;;
*)
  ECHO_N='-n';;
esac

# some terminal codes ...
boldface="`tput bold 2>/dev/null`"
normal="`tput sgr0 2>/dev/null`"
printbold() {
    echo $ECHO_N "$boldface" $ECHO_C
    echo "$@"
    echo $ECHO_N "$normal" $ECHO_C
}

printerr() {
    echo "$@" >&2
}

# Usage:
#     compare_versions MIN_VERSION ACTUAL_VERSION
# returns true if ACTUAL_VERSION >= MIN_VERSION
compare_versions() {
    ch_min_version=$1
    ch_actual_version=$2
    ch_status=0
    IFS="${IFS=         }"; ch_save_IFS="$IFS"; IFS="."
    set $ch_actual_version
    for ch_min in $ch_min_version; do
        ch_cur=`echo $1 | sed 's/[^0-9].*$//'`; shift # remove letter suffixes
        if [ -z "$ch_min" ]; then break; fi
        if [ -z "$ch_cur" ]; then ch_status=1; break; fi
        if [ $ch_cur -gt $ch_min ]; then break; fi
        if [ $ch_cur -lt $ch_min ]; then ch_status=1; break; fi
    done
    IFS="$ch_save_IFS"
    return $ch_status
}

# Usage:
#     version_check PACKAGE VARIABLE CHECKPROGS MIN_VERSION SOURCE
# checks to see if the package is available
version_check() {
    vc_package=$1
    vc_variable=$2
    vc_checkprogs=$3
    vc_min_version=$4
    vc_source=$5
    vc_status=1

    vc_checkprog=`eval echo "\\$$vc_variable"`
    if [ -n "$vc_checkprog" ]; then
	printbold "using $vc_checkprog for $vc_package"
	return 0
    fi

    if test "x$vc_package" = "xautomake" -a "x$vc_min_version" = "x1.4"; then
	vc_comparator="="
    else
	vc_comparator=">="
    fi
    printbold "checking for $vc_package $vc_comparator $vc_min_version..."
    for vc_checkprog in $vc_checkprogs; do
	echo $ECHO_N "  testing $vc_checkprog... " $ECHO_C
	if $vc_checkprog --version < /dev/null > /dev/null 2>&1; then
	    vc_actual_version=`$vc_checkprog --version | head -n 1 | \
                               sed 's/^.*[ 	]\([0-9.]*[a-z]*\).*$/\1/'`
	    if compare_versions $vc_min_version $vc_actual_version; then
		echo "found $vc_actual_version"
		# set variables
		eval "$vc_variable=$vc_checkprog; \
			${vc_variable}_VERSION=$vc_actual_version"
		vc_status=0
		break
	    else
		echo "too old (found version $vc_actual_version)"
	    fi
	else
	    echo "not found."
	fi
    done
    if [ "$vc_status" != 0 ]; then
	printerr "***Error***: You must have $vc_package $vc_comparator $vc_min_version installed"
	printerr "  to build $PKG_NAME.  Download the appropriate package for"
	printerr "  from your distribution or get the source tarball at"
        printerr "    $vc_source"
	printerr
	exit $vc_status
    fi
    return $vc_status
}

existence_check () {
  printbold checking for $1...
  if ! which $1 >/dev/null; then
    printerr "***Error***: You must have $1 installed to hack on $PKG_NAME."
  fi
}

