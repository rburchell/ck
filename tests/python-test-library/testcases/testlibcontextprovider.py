from ctypes import *
import dbus
import os

import unittest
from time import sleep

# FIXME: Use the fake dbus.

class LibraryTestCase(unittest.TestCase):
    def setUp(self):
        print "CWD", os.getcwd()
        self.libc = CDLL("libcontextprovider.so")
        self.STRING_ARRAY = POINTER(c_char_p)
        self.STRING_SET = c_void_p
        self.CHANGE_SET = c_void_p
        self.GET_CALLBACK = CFUNCTYPE(self.STRING_SET, self.CHANGE_SET, c_void_p)
        self.SUBSCRIBE_CALLBACK = CFUNCTYPE(self.STRING_SET, c_void_p)
        self.libc.context_provider_init.argtypes = [self.STRING_ARRAY, self.GET_CALLBACK, c_void_p, self.SUBSCRIBE_CALLBACK, c_void_p, self.SUBSCRIBE_CALLBACK, c_void_p]
    def tearDown(self):
        pass


class Startup(LibraryTestCase):
    def test_startProvidingKeys(self):
        bus = dbus.SessionBus()
        # Start a listener which listens to dbus NameOwnerChanged signals
        os.system("python tests/python-test-library/stubs/dbus_listener.py &")
        sleep(1)
        # Command the listener to listen to FakeProvider
        listener_proxy = bus.get_object("org.freedesktop.ContextKit.Testing.Listener","/org/freedesktop/ContextKit/Testing/Listener")
        listener_iface = dbus.Interface(listener_proxy, "org.freedesktop.ContextKit.Testing.Listener")
        listener_iface.SetTarget("org.freedesktop.FakeProvider")
        # Start a provider stub
        os.system("python tests/python-test-library/stubs/provider_stub.py &")
        sleep(1)
        # Command the provider stub to start exposing services over dbus
        provider_proxy = bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Testing/Provider")
        provider_iface = dbus.Interface(provider_proxy, "org.freedesktop.ContextKit.Testing.Provider")
        provider_iface.Start()
        
        # Get the log from the listener
        log = listener_iface.GetLog()
        
        # Stop the listener
        listener_iface.Exit()
        # Stop the provider
        provider_iface.Exit()
        
        self.assert_(log == "(TargetAppeared)")
        

class ChangeSets(LibraryTestCase):
    def test_createChangeSet(self):
        # FIXME: This functionality is missing from the lib!
        self.libc.context_provider_init (['Context.Test.keyInt'], None, None, None, None, None, None)
        changeset = self.libc.context_provider_change_set_create()
        self.assert_(ret != None)
        ret = self.libc.context_provider_change_set_add_int(ret, "Context.Test.keyInt", 5)
        self.assert_(ret == 0) # Success return value
        ret = self.libc.context_provider_change_set_send_notification(ret)
        self.assert_(ret == 0) # Success return value
        ret = self.libc.context_change_set_delete(ret)

class KeyCounting(LibraryTestCase):
    def test_initialCount(self):
        # Initial count of a key is zero
        
        i = c_int()
        ret = self.libc.context_provider_no_of_subscribers("Context.Test.keyInt", byref(i))
        
        self.assert_(ret == 0) # Success return value
        self.assert_(i.value == 0) # 0 subscribers

def runTests():
    suiteStartup = unittest.TestLoader().loadTestsFromTestCase(Startup)
    suiteChangeSets = unittest.TestLoader().loadTestsFromTestCase(ChangeSets)
    suiteKeyCounting = unittest.TestLoader().loadTestsFromTestCase(KeyCounting)
    
    unittest.TextTestRunner(verbosity=2).run(suiteStartup)
    #unittest.TextTestRunner(verbosity=2).run(suiteChangeSets)
    #unittest.TextTestRunner(verbosity=2).run(suiteKeyCounting)

if __name__ == "__main__":
    # Start listening to NameOwnerChanged dbus signal
    #listener = DBusListener()
    
    runTests();
    #libc.context_kit_key_usage_counter_add(None, None)

