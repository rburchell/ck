import dbus
import sys
import os
import servicetest
from testhelpers import install_colourer
from twisted.internet import reactor

def exec_test_deferred (funs, params, protocol=None, timeout=None):
    colourer = None

    if sys.stdout.isatty():
        colourer = install_colourer()

    queue = servicetest.IteratingEventQueue(timeout)
    queue.verbose = (
        os.environ.get('CHECK_DBUS_VERBOSE', '') != ''
        or '-v' in sys.argv)

    bus = dbus.SessionBus()
    servicetest.setup_dbus(bus, queue.append)

    error = None

    try:
        for f in funs:
            f(queue, bus)
    except Exception, e:
        import traceback
        traceback.print_exc()
        error = e

    if colourer:
        sys.stdout = colourer.fh

    if error is None:
        reactor.stop()
    else:
        os._exit(1)


def exec_tests(funs, params=None, protocol=None, timeout=None):
  reactor.callWhenRunning (exec_test_deferred, funs, params, protocol, timeout)
  reactor.run()

def exec_test(fun, params=None, protocol=None, timeout=None):
  exec_tests([fun], params, protocol, timeout)

def get_manager(bus, name):
    manager_obj = bus.get_object(name, '/org/freedesktop/ContextKit/Manager')
    manager = dbus.Interface(manager_obj, dbus_interface='org.freedesktop.ContextKit.Manager')
    return manager

def get_subscriber(bus, name, path):
    subscriber_obj = bus.get_object(name, path)
    subscriber = dbus.Interface(subscriber_obj, dbus_interface='org.freedesktop.ContextKit.Subscriber')
    return subscriber

def test(q, bus):
    pass

if __name__ == '__main__':
    from contexttest import exec_test
    exec_test(test)
