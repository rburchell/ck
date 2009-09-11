#!/bin/bash

cd $(dirname $0)
DIRS="commander subscription asynchronicity registry bluez-plugin2 pluginchanging"
export PYTHONPATH="`pwd`/common/"

if pkg-config contextprovider-1.0 || [ -e ../../libcontextprovider/src/.libs/libcontextprovider.so ]
then
	export CONTEXT_PROVIDERS=.
	export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextprovider/src/.libs:../testplugins/timeplugin1/.libs:../testplugins/timeplugin2/.libs
	export PATH=$PATH:../../../python:../../cli:../../reg-cli
	export CONTEXT_SUBSCRIBER_PLUGINS=../../plugins/bluez/.libs/

	for dir in $DIRS; do
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
