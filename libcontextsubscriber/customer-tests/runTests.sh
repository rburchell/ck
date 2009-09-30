#!/bin/bash

cd $(dirname $0)
DIRS="commander subscription asynchronicity registry pluginchanging"
. ./env.sh


if pkg-config contextprovider-1.0 || [ -e ../../libcontextprovider/src/.libs/libcontextprovider.so ]
then
	for dir in $DIRS; do
		echo "Running tests in $dir"
		cd $dir
		for file in *.py; do
			python2.5 $file || exit 1
		done
		cd ..
	done
else
	echo "libcontextprovider is not installed nor built"
	exit 1
fi
