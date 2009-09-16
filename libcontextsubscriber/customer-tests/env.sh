#!/bin/sh

export PYTHONPATH="`pwd`/common/"
export CONTEXT_PROVIDERS=.
export LD_LIBRARY_PATH=../../src/.libs:../../../libcontextprovider/src/.libs
export PATH=$PATH:../../../python:../../cli:../../reg-cli
