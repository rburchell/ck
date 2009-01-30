#!/usr/bin/env python2.5
##
## @file contextOrientationTCs.py
##
## Copyright (C) 2008 Nokia. All rights reserved.
##
## 
## 
##
## Requires python2.5-gobject and python2.5-dbus
##
## Implements also some testing API:
##
## 
contextSrcPath="."
sessionConfigPath="tests/python-test-library/stubs"

ctxBusName = "org.freedesktop.ContextKit"
ctxMgrPath = "/org/freedesktop/ContextKit/Manager"
ctxMgrIfce = "org.freedesktop.ContextKit.Manager"

scriberOnePath = "/org/freedesktop/ContextKit/Subscribers/0"
scriberTwoPath = "/org/freedesktop/ContextKit/Subscribers/1"
scriberThirdPath = "/org/freedesktop/ContextKit/Subscribers/2"