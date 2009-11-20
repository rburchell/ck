#!/bin/sh

export PYTHONPATH=${PYTHONPATH:+$PYTHONPATH:}../../../python
export CONTEXT_PROVIDERS=.
export CONTEXT_CORE_DECLARATIONS=../../../spec/core.context
export CONTEXT_CORE_TYPES=../../../spec/core.types
export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextprovider/src/.libs:$LD_LIBRARY_PATH
export PATH=../../../python:../../../libcontextprovider/context-provide:../../cli:../../cls:$PATH
