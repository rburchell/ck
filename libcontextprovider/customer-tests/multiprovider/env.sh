#!/bin/sh

export PYTHONPATH=$PYTHONPATH:../../../python
export CONTEXT_PROVIDERS=.
export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextsubscriber/src/.libs
export PATH=../../../python:../../context-provide:../../../libcontextsubscriber/cli:../../../libcontextsubscriber/cls:../client:$PATH