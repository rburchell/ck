#!/bin/bash

output=`dpkg-query -W -f='${Status}\n' libcontextprovider0`

if [ "$output" != "deinstall ok config-files" -o ! -d ../../libcontextprovider/.libs ]
then
	echo "libcontextprovider is not installed nor built"
	exit 1
else
	export LD_LIBRARY_PATH=../src/.libs:../../libcontextprovider/.libs;
	export PATH=$PATH:../../python:../cli;
	for file in *.py ; do
		./$file
	done
fi
