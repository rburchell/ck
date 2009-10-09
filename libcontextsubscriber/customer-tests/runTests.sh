#!/bin/bash

cd $(dirname $0)
DIRS="commander subscription asynchronicity registry pluginchanging"
. ./env.sh

if [ -n "$COVERAGE" ]
then
    make -C coverage-build
    export LD_LIBRARY_PATH=../coverage-build/.libs:$LD_LIBRARY_PATH
    rm -rf coverage-build/.libs/*.gcda
else
    make -C ../src
fi

make -C testplugins

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

echo "Running update-contextkit-providers customer check"
( cd update-contextkit-providers && ./test.sh )

if [ -n "$COVERAGE" ]
then
    echo "computing coverage"
    rm -rf coverage
    mkdir -p coverage
    lcov --directory coverage-build/.libs/ --capture --output-file coverage/all.cov
    lcov --extract coverage/all.cov '*/src/*.cpp' --output-file coverage/src.cov
    genhtml -o coverage/ coverage/src.cov
fi
