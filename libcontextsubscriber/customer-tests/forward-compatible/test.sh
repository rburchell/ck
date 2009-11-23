#!/bin/sh -e

export CONTEXT_CORE_DECLARATIONS=/dev/null
export CONTEXT_PROVIDERS=./
PATH=.:/usr/lib/contextkit/subscriber-check-version:$PATH
check-version
echo "All ok!"
exit 0
