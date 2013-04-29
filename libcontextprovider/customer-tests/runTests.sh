#!/bin/sh -e

cd $(dirname $0)
DIRS="subscription value-changes types c-api service"

make -C client

export LD_LIBRARY_PATH=../../src/.libs:$LD_LIBRARY_PATH
make -C ../src

for i in $DIRS
do
	make -C $i check-customer
done

make -C ../../libcontextsubscriber/src

. ./env.sh
echo "Running multiproviders customer check"
cd multiprovider; python2.5 multiprovider.py ; cd ..
echo "Running types customer check"
cd types; python2.5 types.py ; cd ..

