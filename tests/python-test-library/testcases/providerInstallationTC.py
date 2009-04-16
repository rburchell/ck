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

from contexttest import exec_test, get_manager, get_subscriber
from servicetest import ProxyWrapper

busname = "org.freedesktop.ContextKit.Testing.Provider.Command"

def test(q,bus):
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    
    
    
    vals,unavail = subscriber.Subscribe(['Context.Example.FaceUp', 'Context.Example.EdgeUp']);
    assert('Context.Example.FaceUp' in (vals.keys() + unavail))
    assert('Context.Example.EdgeUp' in (vals.keys() + unavail))
    vals,unavail = subscriber.Subscribe(['Context.Example.IsDark', 'Context.Example.Count', 'Context.Screen.TopEdge']);
    assert('Context.Example.IsDark' in (vals.keys() + unavail))
    assert('Context.Example.Count' in (vals.keys() + unavail))
    assert('Context.Screen.TopEdge' in (vals.keys() + unavail))
    vals,unavail = subscriber.Subscribe(['Context.Screen.IsCovered']);
    assert('Context.Screen.IsCovered' in (vals.keys() + unavail))

    subscriber2_path = manager.GetSubscriber()
    assert(subscriber2_path == subscriber_path)

    subscriber.Unsubscribe(['Context.Screen.IsCovered', 'Context.Screen.TopEdge'])
    subscriber.Unsubscribe(['Context.Example.IsDark', 'Context.Example.Count','Context.Example.FaceUp', 'Context.Example.EdgeUp'])




def test(q,bus):
    e = SysfsEmulator(q)
    e.add_file("/sys/class/i2c-adapter/i2c-3/3-001d/scale", "normal")
    e.add_file("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "800 800 750")

    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(["Context.Example.FaceUp"]);
    assert(vals.has_key('Context.Example.FaceUp'))
    assert(vals['Context.Example.FaceUp'] == 1)
    assert(len(unavail) == 0)

    # Write new accelerometer values
    e.write("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "800 800 -800")
    # x = 800, y = 800, z = -800 -> FaceUp = false, EdgeUp = 1 (top)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
            args = [{u'Context.Example.FaceUp': 0}, []])

    # Write new accelerometer values
    e.write("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "800 800 750")
    # x = 800, y = 800, z = 750 -> FaceUp = true, EdgeUp = 1 (top)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
            args = [{u'Context.Example.FaceUp': 1}, []])

    vals,unavail = subscriber.Subscribe(['Context.Example.EdgeUp', 'Context.Screen.TopEdge']);
    q.log('Vals: %s' % repr(vals));
    q.log('Unavail: %s' % repr(unavail));
    assert(vals.has_key('Context.Example.EdgeUp'))
    assert(vals['Context.Example.EdgeUp'] == 1)
    assert(vals.has_key('Context.Screen.TopEdge'))
    assert(vals['Context.Screen.TopEdge'] == "top")

    # Write new accelerometer values
    e.write("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "-720 750 0")
    # x = -720, y = 750, z = 0 -> FaceUp = true, EdgeUp = 4 (bottom)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
            args = [{u'Context.Example.EdgeUp': 4, u'Context.Screen.TopEdge':'bottom'}, []])

    # Write new accelerometer values
    e.write("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "750 -20 0")
    # x = 750, y = -20 (below threshold), z = 0 -> FaceUp = true, EdgeUp = 2 (left)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
            args = [{u'Context.Example.EdgeUp': 2, u'Context.Screen.TopEdge':"left"}, []])

    # Write new accelerometer values
    e.write("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "-750 20 0")
    # x = -750, y = 20 (below threshold), z = 0 -> FaceUp = true, EdgeUp = 3 (right)
    q.expect('dbus-signal', signal='Changed', interface='org.freedesktop.ContextKit.Subscriber',
            args = [{u'Context.Example.EdgeUp': 3, u'Context.Screen.TopEdge':"right"}, []])

def test(q,bus):
    manager = get_manager(bus, busname)
    subscriber_path = manager.GetSubscriber()
    assert (subscriber_path != "")
    subscriber = get_subscriber(bus, busname, subscriber_path)
    vals,unavail = subscriber.Subscribe(['Context.Example.FaceUp', 'Context.Example.EdgeUp']);
    assert('Context.Example.FaceUp' in (vals.keys() + unavail))
    assert('Context.Example.EdgeUp' in (vals.keys() + unavail))
    vals,unavail = subscriber.Subscribe(['Context.Example.IsDark', 'Context.Example.Count', 'Context.Screen.TopEdge']);
    assert('Context.Example.IsDark' in (vals.keys() + unavail))
    assert('Context.Example.Count' in (vals.keys() + unavail))
    assert('Context.Screen.TopEdge' in (vals.keys() + unavail))
    vals,unavail = subscriber.Subscribe(['Context.Screen.IsCovered']);
    assert('Context.Screen.IsCovered' in (vals.keys() + unavail))

    subscriber2_path = manager.GetSubscriber()
    assert(subscriber2_path == subscriber_path)

    subscriber.Unsubscribe(['Context.Screen.IsCovered', 'Context.Screen.TopEdge'])
    subscriber.Unsubscribe(['Context.Example.IsDark', 'Context.Example.Count','Context.Example.FaceUp', 'Context.Example.EdgeUp'])
   
if __name__ == '__main__':
    exec_test(test)
