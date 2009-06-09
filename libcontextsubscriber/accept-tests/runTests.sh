#!/bin/bash

if pkg-config contextprovider-1.0 || [ -e ../../libcontextprovider/.libs/libcontextprovider.so ]
then
	export CONTEXT_PROVIDERS=.
	export LD_LIBRARY_PATH=../src/.libs:../../libcontextprovider/.libs
	export PATH=$PATH:../../python:../cli
	for file in *.py ; do
		./$file || exit 1
	done
else
	echo "libcontextprovider is not installed nor built"
	exit 1
fi
