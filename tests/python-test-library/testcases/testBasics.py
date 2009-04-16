#!/usr/bin/env python2.5
##
## @file contextOrientationTCs.py
## This file is part of ContextKit.
##
## Copyright (C) 2008, 2009 Nokia. All rights reserved.
##
## Contact: Marius Vollmer <marius.vollmer@nokia.com>
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public License
## version 2.1 as published by the Free Software Foundation.
##
## This library is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
## 02110-1301 USA
## 
##
## Requires python2.5-gobject and python2.5-dbus
##
import sys
sys.path.append("./testfw/")
import dbus
import conf as cfg
from subprocess import Popen
from time import sleep
from contexttest import exec_tests, get_manager, get_subscriber
from servicetest import ProxyWrapper

busname = "org.freedesktop.ContextKit.Testing.Provider"
#org.freedesktop.ContextKit.Testing.Provider.Command
#org.freedesktop.ContextKit.Testing.Provider

provider_iface=None

def startUp():
    global provider_iface
    bus = dbus.SessionBus()
    initOk = True
        
    # Start a provider stub
    provider_stub_p = Popen("stubs/provider_stub.py", shell=True)
    sleep(0.5)

    try:
        # Command the provider stub to start exposing services over dbus
        provider_proxy = bus.get_object(cfg.fakeProviderBusName, cfg.fakeProviderPath)
        provider_iface = dbus.Interface(provider_proxy, cfg.fakeProviderIfce)
        provider_iface.DoInit(True)
    except:
        print "Exception when executing DoInit"
        initOk = False
        return

    sleep(0.5)

def test(q,bus):
    provider_iface.DoSubscribe()
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.int'])
    assert(len(unavail) == 1)
    propertiesToSend = {"test.int" : 1}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', args = [{u'test.int': 1}, []])

def testTypes(q,bus):
    
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.int'])
    propertiesToSend = {"test.int" : 2}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', args = [{u'test.int': 1}, []])
    
  
if __name__ == '__main__':
    startUp()
    exec_tests([test,testTypes])
