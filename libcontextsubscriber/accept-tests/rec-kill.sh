#!/bin/bash

dokill() {
    for cpid in $(ps -o pid= --ppid $1)
    do
        dokill $cpid
    done
    #echo "killing: $(ps -p $1 -o cmd=)"
    kill -9 $1 > /dev/null 2>&1
}

if [[ $# == 0 ]]; then
    echo "usage: $(basename $0) <top pid to kill>"
    exit 1
fi

for pid in $*
do
    dokill $pid
done
