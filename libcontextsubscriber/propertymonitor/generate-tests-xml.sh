#!/bin/bash -e

if [ $# -ne 4 ]
then
    echo "Usage: generate-tests-xml.sh COREPATH BEGIN_TEMPLATE MIDDLE_TEMPLATE END_TEMPLATE"
    exit 1
fi

which context-ls

# Beginning of the tests.xml template
cat $2 > tests.xml

export CONTEXT_CORE_DECLARATIONS=$1
#export CONTEXT_LOG_VERBOSITY=DEBUG
# core properties start with a capital letter; list them
echo "all keys:"
context-ls
echo "core keys:"
context-ls | grep ^[A-Z]
echo "core keys sorted:"
context-ls | grep ^[A-Z] | sort
for prop in `context-ls | grep ^[A-Z] | sort`
do
    # for each core property, create a test case
    sed "s/PROPNAME/$prop/g" $3 >> tests.xml
done

# End of the tests.xml template
cat $4 >> tests.xml
