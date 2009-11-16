#!/bin/bash

export PYTHONPATH=${PYTHONPATH:+$PYTHONPATH:}../../../python
export CONTEXT_PROVIDERS=.
export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextsubscriber/src/.libs:$LD_LIBRARY_PATH
export PATH=../../../python:../../context-provide:../../../libcontextsubscriber/cli:../../../libcontextsubscriber/cls:../client:$PATH
