#!/bin/sh

cd $(dirname $0)
mkdir -p m4
autoreconf --force --install
./configure --enable-maintainer-mode
