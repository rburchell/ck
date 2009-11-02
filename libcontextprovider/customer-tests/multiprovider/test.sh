#!/bin/bash -e

for file in *.py; do
	python2.5 $file
done