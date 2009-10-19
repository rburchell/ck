#!/bin/bash -e

cat tests.xml.begin > tests.xml

export CONTEXT_CORE_DECLARATIONS=../../spec/core.context
# core properties start with a capital letter; list them
for prop in `../cls/context-ls | grep ^[A-Z] | sort`
do
    # for each core property, create a test case
    sed "s/PROPNAME/$prop/g" tests.xml.middle >> tests.xml
done

cat tests.xml.end >> tests.xml
