#!/bin/sh

export PYTHONPATH=$PYTHONPATH:../../../python
export CONTEXT_PROVIDERS=.
export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextprovider/src/.libs
export PATH=../../../python:../../../libcontextprovider/context-provide:../../cli:../../reg-cli:$PATH
