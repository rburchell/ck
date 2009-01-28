from ctypes import *
import dbus
import os

import unittest
from time import sleep

# FIXME: Use the fake dbus.

class LibraryTestCase(unittest.TestCase):
    def setUp(self):
        self.libc = CDLL("libcontextprovider.so.0.0.0")

    def tearDown(self):
        pass


class Startup(LibraryTestCase):
    def test_startProvidingKeys(self):
        # Start a listener which listens to dbus NameOwnerChanged signals
        #os.system("python tests/python-test-library/stubs/dbus_listener.py &")
        #sleep(1)
        # Start a provider stub
        os.system("python tests/python-test-library/stubs/provider_stub.py &")
        sleep(1)
        # Command the provider stub to start exposing services over dbus
        bus = dbus.SessionBus()
        provider_proxy = bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Testing/Provider")
        provider_iface = dbus.Interface(provider_proxy, "org.freedesktop.ContextKit.Testing.Provider")
        provider_iface.Start()
        
        provider_iface.Exit()
        

class ChangeSets(LibraryTestCase):
    def test_createChangeSet(self):
        # FIXME: Init!
        ret = self.libc.context_changeset_create()
        self.assert_(ret != None)
        ret = self.libc.context_changeset_add_int(ret, "Context.Test.keyInt", 5)
        self.assert_(ret == 0) # Success return value
        ret = self.libc.context_changeset_send_notification(ret)
        self.assert_(ret == 0) # Success return value
        ret = self.libc.context_changeset_delete(ret)

class KeyCounting(LibraryTestCase):
    def test_initialCount(self):
        # Initial count of a key is zero
        
        i = c_int()
        ret = self.libc.context_no_of_subscribers("Context.Test.keyInt", byref(i))
        
        self.assert_(ret == 0) # Success return value
        self.assert_(i.value == 0)

def runTests():
    suiteStartup = unittest.TestLoader().loadTestsFromTestCase(Startup)
    suiteChangeSets = unittest.TestLoader().loadTestsFromTestCase(ChangeSets)
    suiteKeyCounting = unittest.TestLoader().loadTestsFromTestCase(KeyCounting)
    
    #unittest.TextTestRunner(verbosity=2).run(suiteStartup)
    unittest.TextTestRunner(verbosity=2).run(suiteChangeSets)
    #unittest.TextTestRunner(verbosity=2).run(suiteKeyCounting)

if __name__ == "__main__":
    # Start listening to NameOwnerChanged dbus signal
    #listener = DBusListener()
    
    runTests();
    #libc.context_kit_key_usage_counter_add(None, None)

