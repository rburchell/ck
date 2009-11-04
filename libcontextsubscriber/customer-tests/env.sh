#!/bin/sh

export PYTHONPATH=$PYTHONPATH:../../../python
export CONTEXT_PROVIDERS=.
export CONTEXT_CORE_DECLARATIONS=../../../spec/core.context
export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextprovider/src/.libs
export PATH=../../../python:../../../libcontextprovider/context-provide:../../cli:../../cls:$PATH
