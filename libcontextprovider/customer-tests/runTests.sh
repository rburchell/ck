#!/bin/sh -e

DIRS="subscription value-changes types c-api"

make -C client

if [ -n "$COVERAGE" ]
then
    make -C coverage-build
    export LD_LIBRARY_PATH=../coverage-build/.libs:$LD_LIBRARY_PATH
    rm -rf coverage-build/.libs/*.gcda
else
    export LD_LIBRARY_PATH=../../src/.libs:$LD_LIBRARY_PATH
    make -C ../src
fi

for i in $DIRS
do
	make -C $i check-customer
done

if [ -n "$COVERAGE" ]
then
    echo "computing coverage"
    rm -rf coverage
    mkdir -p coverage
    lcov --directory coverage-build/.libs/ --capture --output-file coverage/all.cov
    lcov --extract coverage/all.cov '*/src/*.cpp' --output-file coverage/src.cov
    genhtml -o coverage/ coverage/src.cov
fi
