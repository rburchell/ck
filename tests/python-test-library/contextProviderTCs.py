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
import os

busname = "org.freedesktop.ContextKit.Testing.Provider"

provider_iface=None

def setUp():
    global provider_iface
    bus = dbus.SessionBus()
    initOk = True

    # Make the provider stub to find the libcontextprovider.so
    # (if it is not installed)
    os.environ['LD_LIBRARY_PATH'] = '../../libcontextprovider/.libs'
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

def testInstallGroup(q,bus):
    print "testInstallGroup"
    provider_iface.DoInstall()
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.int'])
    assert(len(vals.keys()) == 0)
    assert(len(unavail) == 1)
    assert(unavail[0] == 'test.int')
    propertiesToSend = {"test.int" : 1}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', args = [{u'test.int': 1}, []])

def testUndeterminable(q,bus):
    print "testUndeterminable"
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.int','test.double'])
    assert(len(vals.keys()) == 1)
    assert(vals['test.int'] == 1)
    assert(len(unavail) == 1)
    assert(unavail[0] == 'test.double')
    propertiesToSend = {}
    undeterminedToSend = ['test.int']
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', args = [{}, [u'test.int']])
        
def testUnsubscribe(q,bus):
    print "testUnsubscribe"
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.double','test.int'])
    assert(len(unavail) == 2)
    assert('test.double' in unavail)
    assert('test.int' in unavail)
    subscriber.Unsubscribe(['test.double'])
    propertiesToSend = {"test.double" : 9.0, "test.int" : 3}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', args = [{u'test.int': 3}, []]) 
    
def testInexistentProperty(q,bus):
    print "testInexistentProperty"
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.inexistent','test.bool'])
    assert(not ('test.inexistent' in unavail))
    assert(not ('test.inexistent' in vals))
    assert(len(unavail) == 1)
    assert('test.bool' in unavail)
    propertiesToSend = {"test.bool" : True, "test.inexistent" : 1}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', args = [{u'test.bool': True}, []]) 
    
def testTypes(q,bus):
    print "testTypes"
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.int','test.double','test.string','test.bool'])
    assert(len(unavail) == 1)
    assert('test.string' in unavail)
    assert(len(vals.keys()) == 3)
    assert('test.int' in vals.keys())
    assert('test.bool' in vals.keys())
    assert('test.double' in vals.keys())
    propertiesToSend = {"test.int" : -8, "test.double" : 0.2, "test.string" : "foo", "test.bool" : False}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', 
             args = [{u'test.int': -8, u'test.double': 0.2 ,u'test.string': "foo", u'test.bool': False}, []])
    subscriber.Unsubscribe(['test.int','test.double','test.string','test.bool'])

def testCallback(q,bus):
    print "testCallback"
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.log','test.string'])
    assert(vals['test.log']=="Subscribed")
    subscriber.Unsubscribe(['test.string'])
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber', 
             args = [{u'test.log': "Unsubscribed"}, []])
    

def testInstallKey(q,bus):
    provider_iface.DoInstallKey()
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test.single.int'])
    assert(len(vals.keys()) == 0)
    assert(len(unavail) == 1)
    assert(unavail[0] == 'test.single.int')
    propertiesToSend = {"test.single.int" : 1}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
              args = [{u'test.single.int': 1}, []])

def testContextPrefixDropping(q,bus):
    print "testContextPrefixDropping"
    provider_iface.DoInstallWithContextPrefix()
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['test2.int','test2.double','test2.string','test2.bool', 'test2.unknown'])

    # Command the provider to change these properties (with the prefix)
    propertiesToSend = {"Context.test2.int" : 109, "Context.test2.double" : 5.89, "Context.test2.string" : "somethingnew", "Context.test2.bool" : True, "Context.test2.unknown": 321}
    undeterminedToSend = []
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)

    # Expected result: we get the Changed signal (even though we're subscribed to
    # keys without the prefix)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
             args = [{u'test2.int': 109, u'test2.double': 5.89 ,u'test2.string': "somethingnew", u'test2.bool': True, u'test2.unknown': 321}, []])

    # Commande the provider to change one property to unknown
    propertiesToSend = {}
    undeterminedToSend = ["Context.test2.unknown"]
    provider_iface.SendChangeSet(propertiesToSend,undeterminedToSend)

    # Expected result: we get the Changed signal again, with the unknown key
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
             args = [{}, ['test2.unknown']])


    subscriber.Unsubscribe(['test2.int','test2.double','test2.string','test2.bool', 'test2.unknown'])


 
if __name__ == '__main__':
    setUp()
    exec_tests([testInstallGroup,testUndeterminable,testInexistentProperty,testUnsubscribe, 
                testTypes,testCallback,testInstallKey, testContextPrefixDropping])
    provider_iface.Exit()
