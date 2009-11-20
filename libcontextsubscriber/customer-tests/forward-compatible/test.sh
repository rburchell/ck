#!/bin/bash -e

export CONTEXT_CORE_DECLARATIONS=/dev/null
export CONTEXT_PROVIDERS=./
./check-version
echo "All ok!"
exit 0
