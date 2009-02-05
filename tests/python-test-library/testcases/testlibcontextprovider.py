import dbus
import os

import unittest
from time import sleep

import sys
sys.path.append("./python/")
import ContextProvider

# FIXME: Use the fake dbus.

# FIXME: Missing testcases
# - Using system bus flag in init
# - Test that callbacks work

class LibraryTestCase(unittest.TestCase):
    def setUp(self):
        pass
    def tearDown(self):
        print "LibraryTestCase tearDown"
        pass


class Startup(LibraryTestCase):
    def setUp(self):
        self.bus = dbus.SessionBus() # Note: using session bus

        # Start a listener which listens to dbus NameOwnerChanged signals
        os.system("python tests/python-test-library/stubs/dbus_listener.py &")
        sleep(0.5)
        # Command the listener to listen to FakeProvider
        self.listener_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Listener","/org/freedesktop/ContextKit/Testing/Listener")
        self.listener_iface = dbus.Interface(self.listener_proxy, "org.freedesktop.ContextKit.Testing.Listener")
        self.listener_iface.SetTarget("org.freedesktop.ContextKit.Testing.Provider")

        # Start a provider stub
        os.system("python tests/python-test-library/stubs/provider_stub.py &")
        sleep(0.5)
        # Command the provider stub to start exposing services over dbus
        self.provider_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider.Command","/org/freedesktop/ContextKit/Testing/Provider")
        self.provider_iface = dbus.Interface(self.provider_proxy, "org.freedesktop.ContextKit.Testing.Provider")
        self.provider_iface.DoInit()
        sleep(0.5)

    def tearDown(self):
        print "Startup tearDown"
        # Stop the provider
        self.provider_iface.Exit()
        sleep(0.5)

        # Stop the listener
        self.listener_iface.Exit()
        pass # FIXME: Call parent class func explicitly

    def test_startProvider(self):
        # Verify that the provider is started and exposed over dbus.

        # Get the log from the listener
        log = self.listener_iface.GetLog()
        self.assert_(log == "(TargetAppeared)")

        # Note: we could also test whether disappearing is noticed
        # but that is dropped for simplicity.


    def test_managerInterfaceImplemented(self):
        # Verify that the provider exposes the Manager interface over dbus properly

        # Try to get the manager object
        proxy_object_manager = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Manager")
        iface_manager = dbus.Interface(proxy_object_manager, "org.freedesktop.ContextKit.Manager")
        self.assert_ (proxy_object_manager != None) # FIXME: What is the correct validity check?
        self.assert_ (iface_manager != None) # FIXME: What is the correct validity check?
        try:
            iface_manager.Get(["temp"])
        except:
            print "Exception caught"
            self.assert_ (False) # Manager interface is not implemented properly
            # After this, only tearDown will be executed


class Subscription(LibraryTestCase):
    def setUp(self):
        # Start a provider stub
        os.system("python tests/python-test-library/stubs/provider_stub.py &")
        sleep(0.5)
        # Command the provider stub to start exposing services over dbus
        bus = dbus.SessionBus()
        self.provider_proxy = bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Testing/Provider")
        self.provider_iface = dbus.Interface(self.provider_proxy, "org.freedesktop.ContextKit.Testing.Provider")
        self.provider_iface.DoInit()
        sleep(0.5)

    def tearDown(self):
        print "Subscription tearDown"
        self.provider_iface.Exit()
        # FIXME: Try to ensure that the stubs are killed properly
        pass # FIXME: Call parent class func explicitly

    def test_subscriberCallbacks(self):
        bus = dbus.SessionBus()
        # Start a subscriber
        #os.system("python tests/python-test-library/stubs/subscriber_stub.py &")
        #sleep(1)


        #subscriber_proxy = bus.get_object("org.freedesktop.ContextKit.Testing.SubscriberStub","/org/freedesktop/ContextKit/Testing/SubscriberStub")
        #subscriber_iface = dbus.Interface(subscriber_proxy, "org.freedesktop.ContextKit.Testing.SubscriberStub")
        #subscriber_iface.DoSubscribe()

        # Verify that the provider got FirstSubscribed event
        # Stop the subscriber

        # Verify that the provider got LastSubscribed event


class ChangeSets(LibraryTestCase):
    def test_createChangeSet(self):
        # FIXME: This functionality is missing from the lib!
        #self.libc.context_provider_init (['Context.Test.keyInt'], None, None, None, None, None, None)
        #changeset = self.libc.context_provider_change_set_create()
        #self.assert_(ret != None)
        #ret = self.libc.context_provider_change_set_add_int(ret, "Context.Test.keyInt", 5)
        #self.assert_(ret == 0) # Success return value
        #ret = self.libc.context_provider_change_set_send_notification(ret)
        #self.assert_(ret == 0) # Success return value
        #ret = self.libc.context_change_set_delete(ret)
        pass

class KeyCounting(LibraryTestCase):
    def test_initialCount(self):
        # Initial count of a key is zero

        #i = c_int()
        #ret = self.libc.context_provider_no_of_subscribers("Context.Test.keyInt", byref(i))

        #self.assert_(ret == 0) # Success return value
        #self.assert_(i.value == 0) # 0 subscribers
        pass

def runTests():
    suiteStartup = unittest.TestLoader().loadTestsFromTestCase(Startup)
    suiteSubscription = unittest.TestLoader().loadTestsFromTestCase(Subscription)
    suiteChangeSets = unittest.TestLoader().loadTestsFromTestCase(ChangeSets)
    suiteKeyCounting = unittest.TestLoader().loadTestsFromTestCase(KeyCounting)

    unittest.TextTestRunner(verbosity=2).run(suiteStartup)
    #unittest.TextTestRunner(verbosity=2).run(suiteSubscription)
    #unittest.TextTestRunner(verbosity=2).run(suiteChangeSets)
    #unittest.TextTestRunner(verbosity=2).run(suiteKeyCounting)

if __name__ == "__main__":
    # Start listening to NameOwnerChanged dbus signal
    #listener = DBusListener()

    runTests()
    #libc.context_kit_key_usage_counter_add(None, None)
