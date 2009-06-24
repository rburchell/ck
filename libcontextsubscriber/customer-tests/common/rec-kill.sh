#!/bin/sh

dokill() {
    for cpid in `ps -o pid= --ppid $1`
    do
        dokill $cpid
    done
    kill -9 $1 > /dev/null 2>&1
}

if test $# = 0
then
    echo "usage: $0 <top pid to kill>"
    exit 1
fi

for pid in $*
do
    dokill $pid
done
