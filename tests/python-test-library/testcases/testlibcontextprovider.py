import dbus
import os
import re
from subprocess import Popen
from signal import SIGKILL

import unittest
from time import sleep

import sys
sys.path.append("./python/") # libcontextprovider python bindings
import ContextProvider

# Test configuration file
import conf as cfg

# Stubs
sys.path.append("./tests/python-test-library/stubs")

# Note: execute this program: libtool --mode=execute -dlopen libcontextprovider.la dbus-launch python tests/python-test-library/testcases/testlibcontextprovider.py

# FIXME: Use the fake dbus.
# FIXME: Make tests run when make check is executed
# FIXME: Code coverage

# FIXME: Missing testcases
# - Removing a provider
# - Getting subscriber twice


class LibraryTestCase(unittest.TestCase):
    def setUp(self):
        pass
    def tearDown(self):
        pass

class Startup(LibraryTestCase):
    def setUp(self):
        self.bus = dbus.SessionBus() # Note: using session bus

        self.initOk = True
        # Start a listener which listens to dbus NameOwnerChanged signals
        self.dbus_listener_p = Popen("python tests/python-test-library/stubs/dbus_listener.py", shell=True)

        sleep(0.5)
        try:
            # Command the listener to listen to FakeProvider
            self.listener_proxy = self.bus.get_object("org.freedesktop.ContextKit.Testing.Listener","/org/freedesktop/ContextKit/Testing/Listener")
            self.listener_iface = dbus.Interface(self.listener_proxy, "org.freedesktop.ContextKit.Testing.Listener")
            self.listener_iface.SetTarget(cfg.fakeProviderLibBusName)
        except:
            self.initOk = False
            return

        # Start a provider stub
        self.provider_stub_p = Popen("python tests/python-test-library/stubs/provider_stub.py", shell=True)
        sleep(0.5)

        try:
            # Command the provider stub to start exposing services over dbus
            self.provider_proxy = self.bus.get_object(cfg.fakeProviderBusName, cfg.fakeProviderPath)
            self.provider_iface = dbus.Interface(self.provider_proxy, cfg.fakeProviderIfce)
            self.provider_iface.DoInit(True)
        except:
            print "Exception when executing DoInit"
            self.initOk = False
            return

        sleep(0.5)

    def tearDown(self):
        print "Startup tearDown"
        # Stop the provider
        try:
            self.provider_iface.Exit()
            os.waitpid(self.provider_stub_p.pid, 0)
        except:
            print "Stopping provider failed"
            os.kill (self.provider_stub_p.pid, SIGKILL)

        # Stop the listener
        try:
            self.listener_iface.Exit()
            os.waitpid(self.dbus_listener_p.pid, 0)
        except:
            print "Stopping provider failed"
            os.kill (self.dbus_listener_p.pid, SIGKILL)

    def test_startProvider(self):
        self.assert_ (self.initOk)

        # Verify that the provider is started and exposed over dbus.

        # Get the log from the listener
        try:
            log = self.listener_iface.GetLog()
        except:
            self.assert_ (False) # Listener not working

        self.assert_(log == "(TargetAppeared)")

        # Note: we could also test whether disappearing is noticed
        # but that is dropped for simplicity.

    def test_managerInterfaceImplemented(self):
        self.assert_ (self.initOk)
        # Verify that the provider exposes the Manager interface over dbus properly

        # Try to get the manager object
        manager_proxy = self.bus.get_object(cfg.fakeProviderLibBusName, cfg.ctxMgrPath)
        manager_iface = dbus.Interface(manager_proxy, cfg.ctxMgrIfce)
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
        self.initOk = True
        print "TestCaseUsingProvider setUp"
        self.bus = dbus.SessionBus() # Note: using session bus

        # Start a provider stub
        self.provider_stub_p = Popen("python tests/python-test-library/stubs/provider_stub.py", shell=True)
        sleep(0.5)

        self.provider_proxy = self.bus.get_object(cfg.fakeProviderBusName, cfg.fakeProviderPath)
        self.provider_iface = dbus.Interface(self.provider_proxy, cfg.fakeProviderIfce)

        try:
            # Command the provider stub to start exposing services over dbus
            self.provider_iface.DoInit(True)
            # Install a provider
            self.provider_iface.DoInstall()
        except:
            self.initOk = False

        sleep(0.5)

    def tearDown(self):
        print "TestCaseUsingProvider tearDown"
        try:
            # Stop the provider stub
            self.provider_iface.Exit()
            os.waitpid(self.provider_stub_p.pid, 0)
        except:
            print "Stopping provider stub failed"
            os.kill(self.provider_stub_p.pid, SIGKILL)
        sleep(0.5)

# Test cases: Check that the Get callback function is called properly,
# and that the change set is treated properly.
class GetCallback(TestCaseUsingProvider):
    def test_getCallbackIsCalled(self):
        self.assert_ (self.initOk)

        # Execute Get
        manager_proxy = self.bus.get_object(cfg.fakeProviderLibBusName, cfg.ctxMgrPath)
        manager_iface = dbus.Interface(manager_proxy, cfg.ctxMgrIfce)
        try:
            self.provider_iface.ResetLog()
        except:
            self.assert_ (False) # Provider not working

        try:
            manager_iface.Get(["test.int"])
        except:
            print "Exception caught"
            self.assert_ (False) # Manager interface is not implemented properly

        # Check from the log that the callback was called
        try:
            log = self.provider_iface.GetLog()
        except:
            self.assert_ (False) # Provider not working

        #print "Log is:", log
        self.assert_ (log == "(get_cb(test.int))");

    def test_changeSetIsTreatedProperly(self):
        # Execute Get
        manager_proxy = self.bus.get_object(cfg.fakeProviderLibBusName, cfg.ctxMgrPath)
        manager_iface = dbus.Interface(manager_proxy, cfg.ctxMgrIfce)

        ret = None
        try:
            ret = manager_iface.Get(["test.int", "test.double"])
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
        self.assert_ (undetermined.count("test.double") == 1)

# Test cases: Check that the First subscriber / Last subscriber callback functions
# are called properly.
class SubscribeCallbacks(TestCaseUsingProvider):

    def setUp(self):
        self.initOk = True
        TestCaseUsingProvider.setUp(self)

        # Execute GetSubscriber
        manager_proxy = self.bus.get_object(cfg.fakeProviderLibBusName,cfg.ctxMgrPath)
        manager_iface = dbus.Interface(manager_proxy, cfg.ctxMgrIfce)

        self.subscriber_path = "";

        try:
            self.provider_iface.ResetLog()
        except:
            self.initOk = False

        try:
            self.subscriber_path = manager_iface.GetSubscriber()
        except:
            print "Exception caught"
            self.initOk = False

        subscriber_proxy = self.bus.get_object(cfg.fakeProviderLibBusName, self.subscriber_path)
        self.subscriber_iface = dbus.Interface(subscriber_proxy, cfg.ctxScriberIfce)

    def tearDown(self):
        TestCaseUsingProvider.tearDown(self)
        # Note that the provider is stopped.
        # All information about current subscriptions is reset.

    def test_firstCallbackIsCalled(self):
        self.assert_ (self.initOk)

        # Execute Subscribe
        try:
            self.subscriber_iface.Subscribe(["test.double"])
        except:
            self.assert_ (False) # Subscriber not working

        # Check from the log that the callback was called
        try:
            log = self.provider_iface.GetLog()
        except:
            self.assert_ (False) # Provider not working

        self.assert_ (log == "(get_cb(test.double))(first_cb(test.double))" or log == "(first_cb(test.double))(get_cb(test.double))")
        # Note that as part of subscribe, also get is called. Here we don't care about the order.

    def test_lastCallbackIsCalled(self):

        # Execute Subscribe
        try:
            self.subscriber_iface.Subscribe(["test.bool"])
        except:
            self.assert_ (False) # Subscriber not working

        # Reset log:
        try:
            self.provider_iface.ResetLog()
        except:
            self.assert_ (False) # Provider not working

        # Execute Unsubscribe
        try:
            self.subscriber_iface.Unsubscribe(["test.bool"])
        except:
            self.assert_ (False) # Subscriber handler not working

        # Check from the log that the callback was called
        try:
            log = self.provider_iface.GetLog()
        except:
            self.assert_ (False) # Provider not working

        #print "Log is:", log

        self.assert_ (log == "(last_cb(test.bool, ))")

    def test_remainingKeys(self):

        # Execute Subscribe
        self.subscriber_iface.Subscribe(["test.bool", "test.int"])

        # Reset log:
        self.provider_iface.ResetLog()

        # Execute Unsubscribe
        self.subscriber_iface.Unsubscribe(["test.bool"])

        # Check from the log that the callback was called
        log = self.provider_iface.GetLog()

        #print "Log is:", log

        self.assert_ (log == "(last_cb(test.bool, test.int))")

# Test cases: Check that the subscriber gets the change set when it is sent.
class Subscription(TestCaseUsingProvider):

    def setUp(self):
        self.initOk = True
        TestCaseUsingProvider.setUp(self)

        # Start subscription handler
        self.subscription_handler_p = Popen(cfg.contextSrcPath + os.path.sep + "tests/python-test-library/testcases/subscriptionHandler.py", shell=True)
        sleep(0.5)

        # Connect to subscription handler

        try:
            subscptionHandlerProxy = self.bus.get_object(cfg.scriberBusName, cfg.scriberHandlerPath)
            self.subscription_handler_iface = dbus.Interface(subscptionHandlerProxy, cfg.scriberHandlerIfce)
        except:
            self.initOk = False

    def tearDown(self):
        # Stop the subscription handler
        try:
            self.subscription_handler_iface.loopQuit()
            os.waitpid(self.subscription_handler_p.pid, 0)
        except:
            print "Stopping subscription handler failed"
            os.kill(self.subscription_handler.pid, SIGKILL)
        TestCaseUsingProvider.tearDown(self)


    def test_subscriberGetsInfo(self):
        self.assert_ (self.initOk)

        # Get a subscriber
        subscriber_path_1 = self.subscription_handler_iface.addSubscriber(True, cfg.fakeProviderLibBusName)

        # Tell the subscriber to subscribe to test.int, test.double and test.bool
        properties, undetermined = self.subscription_handler_iface.subscribe(["test.int", "test.double", "test.bool"], subscriber_path_1)

        # Command the fake provider send the change set
        try:
            self.provider_iface.SendChangeSet1()
        except:
            self.assert_ (False) # Provider not working

        sleep(0.5)

        # Ask the subscriber what changes it got
        try:
            properties, undetermined = self.subscription_handler_iface.getChangedProp(subscriber_path_1)

            # Unsubscribe
            self.subscription_handler_iface.unSubscribe(["test.int", "test.double", "test.bool"], subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (properties != None)
        self.assert_ (undetermined != None)

        self.assert_ (len(properties) == 1)
        self.assert_ ("test.double" in properties)
        self.assert_ (properties["test.double"] == 2.13)

        self.assert_ (len(undetermined) == 1)
        self.assert_ (undetermined.count("test.bool") == 1)

    def test_subscriberGetsOnlyRelevantKeys(self):
        self.assert_ (self.initOk)

        # Get a subscriber
        subscriber_path_1 = self.subscription_handler_iface.addSubscriber(True, cfg.fakeProviderLibBusName)

        # Tell the subscriber to subscribe to test.double and test.bool
        try:
            properties, undetermined = self.subscription_handler_iface.subscribe(["test.double", "test.bool"], subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        # Command the fake provider send the change set
        try:
            self.provider_iface.SendChangeSet2()
        except:
            self.assert_ (False) # Provider not working

        sleep(0.5)

        # Ask the subscriber what changes it got
        try:
            properties, undetermined = self.subscription_handler_iface.getChangedProp(subscriber_path_1)

            # Unsubscribe
            self.subscription_handler_iface.unSubscribe(["test.double", "test.bool"], subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (properties != None)
        self.assert_ (undetermined != None)

        self.assert_ (len(properties) == 2)
        self.assert_ ("test.double" in properties)
        self.assert_ (properties["test.double"] == 3.1415)
        self.assert_ ("test.bool" in properties)
        self.assert_ (properties["test.bool"] == False)

        self.assert_ (len(undetermined) == 0)

    def test_subscriberDoesntGetUnsubscribedKeys(self):
        self.assert_ (self.initOk)

        # Get a subscriber
        try:
            subscriber_path_1 = self.subscription_handler_iface.addSubscriber(True, cfg.fakeProviderLibBusName)

            # Tell the subscriber to subscribe to test.int, test.double and test.bool
            properties, undetermined = self.subscription_handler_iface.subscribe(["test.int", "test.double", "test.bool"], subscriber_path_1)

            self.subscription_handler_iface.unSubscribe(["test.int"], subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        # Command the fake provider to send the change set
        try:
            self.provider_iface.SendChangeSet2() # contains test.int, test.double and test.bool
        except:
            self.assert_ (False) # Provider not working

        sleep(0.5)

        try:
            # Ask the subscriber what changes it got
            properties, undetermined = self.subscription_handler_iface.getChangedProp(subscriber_path_1)

            # Unsubscribe from the rest
            self.subscription_handler_iface.unSubscribe(["test.double", "test.bool"], subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (properties != None)
        self.assert_ (undetermined != None)

        #print "Properties:", properties
        #print "Undetermined:", undetermined

        self.assert_ (len(properties) == 2)
        self.assert_ ("test.double" in properties)
        self.assert_ (properties["test.double"] == 3.1415)
        self.assert_ ("test.bool" in properties)
        self.assert_ (properties["test.bool"] == False)

        self.assert_ (len(undetermined) == 0)

# Test cases: functionality related to change sets
# Note that "Subscription" test suite tests that change sets are
# sent to the subscriber properly.
class ChangeSets(TestCaseUsingProvider):

    def setUp(self):
        self.initOk = True
        TestCaseUsingProvider.setUp(self)

        # Start subscription handler
        self.subscription_handler_p = Popen(cfg.contextSrcPath + os.path.sep + "tests/python-test-library/testcases/subscriptionHandler.py", shell=True)
        sleep(0.5)

        # Connect to subscription handler

        try:
            subscptionHandlerProxy = self.bus.get_object(cfg.scriberBusName, cfg.scriberHandlerPath)
            self.subscription_handler_iface = dbus.Interface(subscptionHandlerProxy, cfg.scriberHandlerIfce)

            # Get a subscriber
            self.subscriber_path_1 = self.subscription_handler_iface.addSubscriber(True, cfg.fakeProviderLibBusName)

            # Tell the subscriber to subscribe to all the properties
            self.subscription_handler_iface.subscribe(["test.int", "test.double", "test.bool"], self.subscriber_path_1)
        except:
            self.initOk = False
            return

    def tearDown(self):
        # Stop the subscription handler
        try:
            self.subscription_handler_iface.loopQuit()
            os.waitpid(self.subscription_handler_p.pid, 0)
        except:
            print "Stopping subscription handler failed"
            os.kill(self.subscription_handler.pid, SIGKILL)

        TestCaseUsingProvider.tearDown(self)

    def test_dataTypes(self):

        self.assert_ (self.initOk)

        # Command the fake provider to send the change set
        try:
            self.provider_iface.SendChangeSetWithAllDataTypes()
        except:
            self.assert_ (False) # Provider not working
        sleep(0.5)

        # Ask the subscriber what changes it got
        try:
            properties, undetermined = self.subscription_handler_iface.getChangedProp(self.subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (properties != None)
        self.assert_ (undetermined != None)

        #print "Properties:", properties
        #print "Undetermined:", undetermined

        self.assert_ (len(properties) == 3)
        self.assert_ ("test.int" in properties)
        self.assert_ (properties["test.int"] == -8)
        self.assert_ ("test.double" in properties)
        self.assert_ (properties["test.double"] == 0.2)
        self.assert_ ("test.bool" in properties)
        self.assert_ (properties["test.bool"] == True)

        self.assert_ (len(undetermined) == 0)

    def test_undetermined(self):

        self.assert_ (self.initOk)

        # Command the fake provider to send the change set
        try:
            self.provider_iface.SendChangeSetWithAllUndetermined()
        except:
            self.assert_ (False) # Provider not working # Undeterministic segfaults here?

        sleep(0.5)

        # Ask the subscriber what changes it got
        try:
            properties, undetermined = self.subscription_handler_iface.getChangedProp(self.subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (properties != None)
        self.assert_ (undetermined != None)

        #print "Properties:", properties
        #print "Undetermined:", undetermined

        self.assert_ (len(properties) == 0)

        self.assert_ (len(undetermined) == 3)
        self.assert_ (undetermined.count("test.int") == 1)
        self.assert_ (undetermined.count("test.double") == 1)
        self.assert_ (undetermined.count("test.bool") == 1)

    def test_cancelling(self):

        self.assert_ (self.initOk)

        try:
            self.subscription_handler_iface.resetSignalStatus(self.subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        try:
            self.provider_iface.CancelChangeSet()
        except:
            self.assert_ (False) # Provider not working
        sleep(0.5)

        try:
            status = self.subscription_handler_iface.getSignalStatus(self.subscriber_path_1)
        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (status == False)

# Test cases: Check that the subscriber counts of the keys are calculated properly.
class KeyCounting(TestCaseUsingProvider):
    def setUp(self):
        self.initOk = True
        TestCaseUsingProvider.setUp(self)

        # Start subscription handler
        self.subscription_handler_p = Popen(cfg.contextSrcPath + os.path.sep + "tests/python-test-library/testcases/subscriptionHandler.py", shell=True)
        sleep(0.5)

        # Connect to subscription handler

        try:
            subscptionHandlerProxy = self.bus.get_object(cfg.scriberBusName, cfg.scriberHandlerPath)
            self.subscription_handler_iface = dbus.Interface(subscptionHandlerProxy, cfg.scriberHandlerIfce)
        except:
            self.initOk = False

    def tearDown(self):
        # Stop the subscription handler
        try:
            self.subscription_handler_iface.loopQuit()
            os.waitpid(self.subscription_handler_p.pid, 0)
        except:
            print "Stopping subscription handler failed"
            os.kill(self.subscription_handler.pid, SIGKILL)

        TestCaseUsingProvider.tearDown(self)

    def test_initialCount(self):

        self.assert_ (self.initOk)

        # Initial count of a key is zero
        self.assert_ (self.provider_iface.GetSubscriberCount("test.int") == 0)
        self.assert_ (self.provider_iface.GetSubscriberCount("test.double") == 0)
        self.assert_ (self.provider_iface.GetSubscriberCount("test.bool") == 0)

    def test_subscriptionIncreasesCount(self):

        self.assert_ (self.initOk)

        # Command the subscriberHandler to subscribe to test.int

        # Get a subscriber
        try:
            subscriber_path_1 = self.subscription_handler_iface.addSubscriber(True, cfg.fakeProviderLibBusName)
            print "Path is:", subscriber_path_1

            # Tell the subscriber to subscribe to test.int
            properties, undetermined = self.subscription_handler_iface.subscribe(["test.int"], subscriber_path_1)
            sleep(0.5)
        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (self.provider_iface.GetSubscriberCount("test.int") == 1)
        self.assert_ (self.provider_iface.GetSubscriberCount("test.double") == 0)
        self.assert_ (self.provider_iface.GetSubscriberCount("test.bool") == 0)

        # Command another subscriberHandler to subscribe to test.int and test.double
        # FIXME: Being able to have 2 independent subscribers not supporeted by the subscription handler
        # Get a subscriber
        #try:
        #    subscriber_path_2 = self.subscription_handler_iface.addSubscriber(True, cfg.fakeProviderLibBusName)
        #    print "Path is:", subscriber_path_2

        #    # Tell the subscriber to subscribe to test.int and test.double
        #    properties, undetermined = self.subscription_handler_iface.subscribe(["test.int", "test.double"], subscriber_path_2)
        #    sleep(0.5)
        #except:
        #    self.assert_ (False) # Subscription handler not working

        #self.assert_ (self.provider_iface.GetSubscriberCount("test.int") == 2)
        #self.assert_ (self.provider_iface.GetSubscriberCount("test.double") == 1)
        #self.assert_ (self.provider_iface.GetSubscriberCount("test.bool") == 0)

class TestCaseUseSystemBus(unittest.TestCase):
    def setUp(self):
        self.initOk = True

        # Create a session bus
        # and put it's address to the "system bus address" environment variable.
        # This way, all child processes trying to use the system bus go there.
        output = os.popen("dbus-launch").readlines()
        if len(output) >= 2:
            self.dbusAddress = re.match("(DBUS_SESSION_BUS_ADDRESS)=(.*)",output[0]).group(2)
            self.dbusDaemonPid = int(re.match("(DBUS_SESSION_BUS_PID)=(.*)",output[1]).group(2))
            print "New session bus address:", self.dbusAddress
            print "Process pid:", self.dbusDaemonPid
            os.environ["DBUS_SYSTEM_BUS_ADDRESS"] = self.dbusAddress

        else:
            print "Failed to start new session bus"
            self.initOk = False


        self.bus = dbus.SessionBus() # Note: using session bus for commanding the stubs

        # Start a provider stub
        self.providerProcess = Popen("python tests/python-test-library/stubs/provider_stub.py", shell=True)
        sleep(0.5)
        # Command the provider stub to start exposing services over dbus
        self.provider_proxy = self.bus.get_object(cfg.fakeProviderBusName, cfg.fakeProviderPath)
        self.provider_iface = dbus.Interface(self.provider_proxy, cfg.fakeProviderIfce)

        try:
            self.provider_iface.DoInit(False) # Note: use the system bus
            # Install a provider
            self.provider_iface.DoInstall()
        except:
            print "Setting up provider stub failed"
            self.initOk = False
            return

        sleep(0.5)

        # Start subscription handler
        self.subscriptionHandlerProcess = Popen("python tests/python-test-library/testcases/subscriptionHandler.py", shell=True)
        sleep(0.5)

        try:
            subscptionHandlerProxy = self.bus.get_object(cfg.scriberBusName, cfg.scriberHandlerPath)
            self.subscription_handler_iface = dbus.Interface(subscptionHandlerProxy, cfg.scriberHandlerIfce)

            # Get a subscriber
            self.subscriber_path_1 = self.subscription_handler_iface.addSubscriber(False, cfg.fakeProviderLibBusName)
            # Note: use the system bus

            # Tell the subscriber to subscribe to all the properties
            self.subscription_handler_iface.subscribe(["test.int", "test.double", "test.bool"], self.subscriber_path_1)
        except:
            print "Setting up subscription handler failed"
            self.initOk = False
            return

    def tearDown(self):
        # Command the subscription handler to exit
        try:
            self.subscription_handler_iface.loopQuit()
            os.waitpid(self.subscriptionHandlerProcess.pid, 0)
        except:
            print "Stopping subscription handler failed"
            os.kill(self.subscriptionHandlerProcess.pid, SIGKILL)

        try:
            # Stop the provider stub
            self.provider_iface.Exit()
            os.waitpid(self.providerProcess.pid, 0)
        except:
            print "Stopping provider stub failed"
            os.kill(self.providerProcess.pid, SIGKILL)

        # Kill the dbus daemon
        #try:
        os.kill(self.dbusDaemonPid, SIGKILL)
        #except:
        #    print "Stopping dbus daemon failed"

        sleep(0.5)

    def test_subscriberGetsInfo(self):

        self.assert_ (self.initOk)

        # Command the fake provider send the change set
        try:
            self.provider_iface.SendChangeSet1()
        except:
            self.assert_ (False) # Provider not working

        sleep(0.5)

        # Ask the subscriber what changes it got
        try:
            properties, undetermined = self.subscription_handler_iface.getChangedProp(self.subscriber_path_1)

        except:
            self.assert_ (False) # Subscription handler not working

        self.assert_ (properties != None)
        self.assert_ (undetermined != None)

        self.assert_ (len(properties) == 1)
        self.assert_ ("test.double" in properties)
        self.assert_ (properties["test.double"] == 2.13)

        self.assert_ (len(undetermined) == 1)
        self.assert_ (undetermined.count("test.bool") == 1)

def runTests():
    suiteStartup = unittest.TestLoader().loadTestsFromTestCase(Startup)
    suiteGetCallback = unittest.TestLoader().loadTestsFromTestCase(GetCallback)
    suiteSubscribeCallbacks = unittest.TestLoader().loadTestsFromTestCase(SubscribeCallbacks)
    suiteChangeSets = unittest.TestLoader().loadTestsFromTestCase(ChangeSets)
    suiteKeyCounting = unittest.TestLoader().loadTestsFromTestCase(KeyCounting)
    suiteSubscription = unittest.TestLoader().loadTestsFromTestCase(Subscription)
    suiteUseSystemBus = unittest.TestLoader().loadTestsFromTestCase(TestCaseUseSystemBus)

    unittest.TextTestRunner(verbosity=2).run(suiteStartup)
    unittest.TextTestRunner(verbosity=2).run(suiteGetCallback)
    unittest.TextTestRunner(verbosity=2).run(suiteSubscribeCallbacks)
    unittest.TextTestRunner(verbosity=2).run(suiteChangeSets)
    unittest.TextTestRunner(verbosity=2).run(suiteKeyCounting)
    unittest.TextTestRunner(verbosity=2).run(suiteSubscription)
    unittest.TextTestRunner(verbosity=2).run(suiteUseSystemBus)

if __name__ == "__main__":
    runTests()
