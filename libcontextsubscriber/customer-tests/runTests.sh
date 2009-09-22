#!/bin/bash

cd $(dirname $0)
DIRS="commander subscription asynchronicity registry pluginchanging"
. ./env.sh


if pkg-config contextprovider-1.0 || [ -e ../../libcontextprovider/src/.libs/libcontextprovider.so ]
then
	# Copy built test plugin .so's closer to the test program
	cp testplugins/timeplugin1/.libs/libcontextsubscribertime1.so.0.0.0 testplugins/
	cp testplugins/timeplugin2/.libs/libcontextsubscribertime2.so.0.0.0 testplugins/

	for dir in $DIRS; do
		echo "Running tests in $dir"
		cd $dir
		for file in *.py; do
			python2.5 $file || exit 1
		done
		cd ..
	done
	rm testplugins/libcontextsubscribertime1.so.0.0.0
	rm testplugins/libcontextsubscribertime2.so.0.0.0

else
	echo "libcontextprovider is not installed nor built"
	exit 1
fi
