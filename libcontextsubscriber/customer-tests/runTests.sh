#!/bin/bash

DIRS="commander subscription asynchronicity"

if pkg-config contextprovider-1.0 || [ -e ../../libcontextprovider/.libs/libcontextprovider.so ]
then
	export CONTEXT_PROVIDERS=.
	export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextprovider/.libs
	export PATH=$PATH:../../../python:../../cli

	for dir in $DIRS; do
		cd $dir
		for file in *.py; do
			./$file || exit 1
		done
		cd ..
	done
else
	echo "libcontextprovider is not installed nor built"
	exit 1
fi
