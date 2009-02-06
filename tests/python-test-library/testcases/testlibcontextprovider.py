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
        manager_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Manager")
        manager_iface = dbus.Interface(manager_proxy, "org.freedesktop.ContextKit.Manager")
        self.assert_ (manager_proxy != None) # FIXME: What is the correct validity check?
        self.assert_ (manager_iface != None) # FIXME: What is the correct validity check?
        try:
            manager_iface.Get(["temp"])
        except:
            print "Exception caught"
            self.assert_ (False) # Manager interface is not implemented properly
            # After this, only tearDown will be executed

# Parent class for testcases which need to start a provider and stop it
class TestCaseUsingProvider(unittest.TestCase):
    def setUp(self):
        print "TestCaseUsingProvider setUp"
        self.bus = dbus.SessionBus() # Note: using session bus

        # Start a provider stub
        os.system("python tests/python-test-library/stubs/provider_stub.py &")
        sleep(0.5)
        # Command the provider stub to start exposing services over dbus
        self.provider_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider.Command","/org/freedesktop/ContextKit/Testing/Provider")
        self.provider_iface = dbus.Interface(self.provider_proxy, "org.freedesktop.ContextKit.Testing.Provider")
        self.provider_iface.DoInit()

        # Install a provider
        self.provider_iface.DoInstall()

        sleep(0.5)

    def tearDown(self):
        print "TestCaseUsingProvider tearDown"
        # Uninstall a provider
        self.provider_iface.DoRemove()
        # Stop the provider stub
        self.provider_iface.Exit()
        sleep(0.5)

# Test cases: Check that the Get callback function is called properly, and that the change set is treated properly.
class GetCallback(TestCaseUsingProvider):
    def test_getCallbackIsCalled(self):
        # Execute Get
        manager_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Manager")
        manager_iface = dbus.Interface(manager_proxy, "org.freedesktop.ContextKit.Manager")

        self.provider_iface.ResetLog()
        try:
            manager_iface.Get(["test.int"])
        except:
            print "Exception caught"
            self.assert_ (False) # Manager interface is not implemented properly

        # Check from the log that the callback was called
        log = self.provider_iface.GetLog()

        self.assert_ (log == "(get_cb(test.int))");

    def test_changeSetIsTreatedProperly(self):
        # Execute Get
        manager_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Manager")
        manager_iface = dbus.Interface(manager_proxy, "org.freedesktop.ContextKit.Manager")

        ret = None
        try:
            ret = manager_iface.Get(["test.int", "test.string"])
        except:
            print "Exception caught"
            self.assert_ (False) # Manager interface is not implemented properly

        self.assert_(ret != None)

        properties = ret[0]
        undetermined = ret[1]

        print "Properties are", properties
        print "Undetermined are", undetermined

        self.assert_ (len(properties) == 1)

        self.assert_ ("test.int" in properties)
        self.assert_ (properties["test.int"] == 5)

        self.assert_ (len(undetermined) == 1)
        self.assert_ (undetermined.count("test.string") == 1)

        #self.assert_ (False); # Test under implementation

class SubscribeCallbacks(TestCaseUsingProvider):

    def setUp(self):
        TestCaseUsingProvider.setUp(self)

    def tearDown(self):
        TestCaseUsingProvider.tearDown(self)

    def test_firstCallbackIsCalled(self):

        # FIXME: Consider refactoring getting the subscriber to the setUp.
        # The problem is that it may fail.

        # Execute GetSubscriber
        manager_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Manager")
        manager_iface = dbus.Interface(manager_proxy, "org.freedesktop.ContextKit.Manager")

        subscriber_path = "";
        self.provider_iface.ResetLog()
        try:
            subscriber_path = manager_iface.GetSubscriber()
        except:
            print "Exception caught"
            self.assert_ (False) # Manager interface is not implemented properly

        self.assert_ (subscriber_path == "/org/freedesktop/ContextKit/Subscribers/0")

        # Execute Subscribe
        subscriber_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider",subscriber_path)
        subscriber_iface = dbus.Interface(subscriber_proxy, "org.freedesktop.ContextKit.Subscriber")

        subscriber_iface.Subscribe(["test.string"])

        # Check from the log that the callback was called
        log = self.provider_iface.GetLog()

        self.assert_ (log == "(get_cb(test.string))(first_cb(test.string))" or log == "(first_cb(test.string))(get_cb(test.string))")
        # Note that as part of subscribe, also get is called. Here we don't care about the order.

    def test_lastCallbackIsCalled(self):
        # Execute GetSubscriber
        manager_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider","/org/freedesktop/ContextKit/Manager")
        manager_iface = dbus.Interface(manager_proxy, "org.freedesktop.ContextKit.Manager")

        subscriber_path = "";
        try:
            subscriber_path = manager_iface.GetSubscriber()
        except:
            print "Exception caught"
            self.assert_ (False) # Manager interface is not implemented properly

        self.assert_ (subscriber_path == "/org/freedesktop/ContextKit/Subscribers/0")

        # Execute Subscribe
        subscriber_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Provider",subscriber_path)
        subscriber_iface = dbus.Interface(subscriber_proxy, "org.freedesktop.ContextKit.Subscriber")

        subscriber_iface.Subscribe(["test.boolean"])

        # Reset log:
        self.provider_iface.ResetLog()

        # Execute Unsubscribe
        subscriber_iface.Unsubscribe(["test.boolean"])

        # Check from the log that the callback was called
        log = self.provider_iface.GetLog()

        self.assert_ (log == "(last_cb(test.boolean))")


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


class ChangeSets(TestCaseUsingProvider):

    def setUp(self):
        TestCaseUsingProvider.setUp(self)

    def tearDown(self):
        TestCaseUsingProvider.tearDown(self)


    def test_changeSetNotifiedToSubscriber(self):
        # Create a subscriber which subscribes to our properties
        # FIXME: implementation missing

        # Create a change set
        self.provider_iface.SendChangeSet1()

        # Check from the subscriber log that the change was done

        self.assert_ (False) # This test is not yet implemented

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
    suiteGetCallback = unittest.TestLoader().loadTestsFromTestCase(GetCallback)
    suiteSubscribeCallbacks = unittest.TestLoader().loadTestsFromTestCase(SubscribeCallbacks)
    suiteChangeSets = unittest.TestLoader().loadTestsFromTestCase(ChangeSets)

    suiteSubscription = unittest.TestLoader().loadTestsFromTestCase(Subscription)

    suiteKeyCounting = unittest.TestLoader().loadTestsFromTestCase(KeyCounting)

    unittest.TextTestRunner(verbosity=2).run(suiteStartup)
    unittest.TextTestRunner(verbosity=2).run(suiteGetCallback)
    unittest.TextTestRunner(verbosity=2).run(suiteSubscribeCallbacks)
    unittest.TextTestRunner(verbosity=2).run(suiteChangeSets)

    #unittest.TextTestRunner(verbosity=2).run(suiteSubscription)

    #unittest.TextTestRunner(verbosity=2).run(suiteKeyCounting)

if __name__ == "__main__":
    # Start listening to NameOwnerChanged dbus signal
    #listener = DBusListener()

    runTests()
    #libc.context_kit_key_usage_counter_add(None, None)
