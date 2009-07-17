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

ctxScriberIfce = "org.freedesktop.ContextKit.Subscriber"

mceBusName = "com.nokia.mce"
mceRqstPath = "/com/nokia/mce/request"
mceRqstIfce = "com.nokia.mce.request"

# Subscriber handler DBus interface
scriberBusName = "org.freedesktop.context.testing.subHandler"
scriberHandlerPath = "/org/freedesktop/context/testing/subHandler/request"
scriberHandlerIfce = "org.freedesktop.context.testing.subHandler.request"

# Fake provider DBus interface
fakeProviderBusName = "org.freedesktop.ContextKit.Testing.Provider.Command";
fakeProviderLibBusName = "org.freedesktop.ContextKit.Testing.Provider";
# Note: two bus names are needed; one object is initialized by the library
# and one is used for commanding the stub.
fakeProviderPath = "/org/freedesktop/ContextKit/Testing/Provider";
fakeProviderIfce = "org.freedesktop.ContextKit.Testing.Provider";

scriberOnePath = "/org/freedesktop/ContextKit/Subscribers/0"
scriberTwoPath = "/org/freedesktop/ContextKit/Subscribers/1"
scriberThirdPath = "/org/freedesktop/ContextKit/Subscribers/2"

properties = ['Context.Device.Orientation.facingUp','Context.Device.Orientation.edgeUp']