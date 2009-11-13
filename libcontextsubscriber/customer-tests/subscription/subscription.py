#!/usr/bin/env python
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
import unittest
import os
from ContextKit.cltool import CLTool, wanted, wantedUnknown

class Subscription(unittest.TestCase):
        def tearDown(self):
                os.unlink('./context-provide.context')

        def testValue(self):
                """
                Description
                        Subscribe to 4 properties covering basic data types
                        Query the value of the property
                        (when it's known or unknown, with and without a default value)

                Pre-conditions
                        Provider started via context-provide tool.
                        4 data types supported int, bool, double and string

                Steps
                        Subscribe to the properties, int, bool, double, string
                        Change value of type int from the provider to a known value
                        Query the value (both with and without a default value) and assert the result
                        Change value of type int from the provider to unknown
                        Query the value (both with and without a default value) and assert the result

                Post-conditions
                        Kill provider

                References
                        None
                """

                provider = CLTool("context-provide", "--v2", "com.nokia.test",
                                 "int","test.int","1",
                                 "string","test.string","foobar",
                                 "double","test.double","2.5",
                                 "truth","test.truth","True")
                provider.send("dump")
                provider.expect("Wrote") # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth")
                self.assert_(
                        listen.expect(["^test.double = double:2.5$",
                                          "^test.int = int:1$",
                                          "^test.string = QString:\"foobar\"$",
                                          "^test.truth = bool:true$"]),
                        "Actual key values pairs do not match expected")

                provider.send("test.int = 100")
                listen.expect("int:100") # wait for it
                listen.send("value test.int")
                self.assert_(
                        listen.expect("^value: int:100$"),
                        "Value command returned wrong value")

                listen.send("value test.int \"defaultValue\"")
                self.assert_(
                        listen.expect("^value: int:100$"),
                        "Value command returned wrong value")

                provider.send("unset test.int")
                listen.expect("Unknown") # wait for it
                listen.send("value test.int")
                self.assert_(
                        listen.expect("^value: Unknown$"),
                        "Value command returned wrong value")

                listen.send("value test.int \"defaultValue\"")
                self.assert_(
                        listen.expect("^value: QString:\"defaultValue\"$"),
                        "Value command returned wrong value")

                listen.send("key test.int")
                self.assert_(
                        listen.expect("^key: test.int$"),
                        "Key command returned wrong value")

                # Because of a python threading / process / CLTool
                # issue, processes need to be waited for in the
                # opposite order as they are started.
                listen.wait()
                provider.wait()

        def testInfos(self):
                """
                Description
                        Subscribe to 4 properties covering basic data types

                Pre-conditions
                        Provider started via context-provide tool.
                        4 data types supported int, bool, double and string

                Steps
                        Subscribe to the properties, int, bool, double, string
                        Assert the returned information for key, description and type

                Post-conditions
                        Kill provider

                References
                        None
                """

                provider = CLTool("context-provide", "--v2", "com.nokia.test",
                                 "int","test.int","1",
                                 "string","test.string","foobar",
                                 "double","test.double","2.5",
                                 "truth","test.truth","True")
                provider.expect("Setting key: test.truth") # wait for it
                provider.send("dump")
                provider.expect("Wrote") # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth")
                listen.expect(["^test.double = double:2.5$",
                                  "^test.int = int:1$",
                                  "^test.string = QString:\"foobar\"$",
                                  "^test.truth = bool:true$"]) # wait for it
                listen.send("ikey test.int")

                self.assert_(
                        listen.expect("^ikey: test.int$"),
                        "ikey didn't work")

                listen.send("man test.truth")
                self.assert_(
                        listen.expect("^man: A phony but very flexible property.$"),
                        "man didn't work")

                listen.send("type test.double")
                self.assert_(
                        listen.expect("^type: DOUBLE$"),
                        "type didn't work")
                listen.wait()
                provider.wait()

        def testTypes(self):
                provider = CLTool("context-provide", "--v2", "com.nokia.test",
                                 "int","test.int","1",
                                 "string","test.string","foobar",
                                 "double","test.double","2.5",
                                 "truth","test.truth","True")
                provider.expect("Setting key: test.truth") # wait for it
                provider.send("dump")
                provider.expect("Wrote") # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth", "test.fake")
                listen.expect(["^test.double = double:2.5$",
                                  "^test.int = int:1$",
                                  "^test.string = QString:\"foobar\"$",
                                  "^test.truth = bool:true$"]) # wait for it

                # test querying the type of all properties
                listen.send("type test.int")
                self.assert_(
                        listen.expect("^type: INT$"))

                listen.send("type test.double")
                self.assert_(
                        listen.expect("^type: DOUBLE$"))

                listen.send("type test.truth")
                self.assert_(
                        listen.expect("^type: TRUTH$"))

                listen.send("type test.string")
                self.assert_(
                        listen.expect("^type: STRING$"))

                listen.send("type test.fake")
                self.assert_(
                        listen.expect("^type:$"))

                listen.wait()
                provider.wait()

        def testProviders(self):
                provider = CLTool("context-provide", "--v2", "com.nokia.test",
                                 "int","test.int","1",
                                 "string","test.string","foobar",
                                 "double","test.double","2.5",
                                 "truth","test.truth","True")
                provider.expect("Setting key: test.truth") # wait for it
                provider.send("dump")
                provider.expect("Wrote") # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth", "test.fake")
                listen.expect(["^test.double = double:2.5$",
                                  "^test.int = int:1$",
                                  "^test.string = QString:\"foobar\"$",
                                  "^test.truth = bool:true$"]) # wait for it

                # test querying the provider(s)
                listen.send("providers test.int")
                self.assert_(
                        listen.expect("^providers: session:com.nokia.test@contextkit-dbus$"))

                listen.send("providers test.fake")
                self.assert_(
                        listen.expect("^providers:$"))
                listen.wait()
                provider.wait()

        def testAllDataTypes(self):
                """
                Description
                        Subscribe to 4 properties covering basic data types

                Pre-conditions
                        Provider started via context-provide tool.
                        4 data types supported int, bool, double and string

                Steps
                        Subscribe to the properties, int, bool, double, string
                        Properties values

                Post-conditions
                        Kill provider

                References
                        None
                """

                provider = CLTool("context-provide", "--v2", "com.nokia.test",
                                 "int","test.int","1",
                                 "string","test.string","foobar",
                                 "double","test.double","2.5",
                                 "truth","test.truth","True")
                provider.expect("Setting key: test.truth") # wait for it
                provider.send("dump")
                provider.expect("Wrote") # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth")
                self.assert_(
                        listen.expect(["^test.double = double:2.5$",
                                          "^test.int = int:1$",
                                          "^test.string = QString:\"foobar\"$",
                                          "^test.truth = bool:true$"]),
                        "Actual key values pairs do not match expected")
                listen.wait()
                provider.wait()

        def testTruthTypePermutations(self):
                """
                Description
                        Subscribe to 4 properties covering basic data types

                Pre-conditions
                        Provider started via context-provide tool.
                        4 data types supported int, bool, double and string

                Steps
                        Subscribe to the properties, int, bool, double, string
                        Change property of type bool to False, None and True
                        Property value of type bool is updated to False, None and True

                Post-conditions
                        Kill provider

                References
                        None
                """
                provider = CLTool("context-provide", "--v2", "com.nokia.test", "truth", "test.truth", "False")
                provider.expect("Setting key: test.truth") # wait for it
                provider.send("dump")
                provider.expect("Wrote") # wait for it
                provider.send("test.truth = False")
                provider.expect("Setting") # wait for it
                listen = CLTool("context-listen", "test.truth")
                self.assert_(
                        listen.expect("^test.truth = bool:false$"),
                        "setting to false didn't work")

                provider.send("unset test.truth")
                self.assert_(
                        listen.expect(wantedUnknown("test.truth")),
                        "setting to unknown didn't work")

                provider.send("test.truth = True")
                self.assert_(
                        listen.expect("^test.truth = bool:true$"),
                        "setting to true didn't work")
                listen.wait()
                provider.wait()

        def testStringTypePermutations(self):
                """
                Description
                        Subscribe to 4 properties covering basic data types

                Pre-conditions
                        Provider started via context-provide tool.
                        4 data types supported int, bool, double and string

                Steps
                        Subscribe to the properties, int, bool, double, string
                        Change property of type string to empty string and None
                        Property value of type bool is updated to empty string and None

                Post-conditions
                        Kill provider

                References
                        None
                """
                provider = CLTool("context-provide", "--v2", "com.nokia.test", "string", "test.string", "something")
                provider.expect("Setting key: test.string") # wait for it
                provider.send("dump")
                provider.expect("Wrote") # wait for it
                listen = CLTool("context-listen", "test.string")

                self.assert_(
                        listen.expect("^test.string = QString:\"something\"$"),
                        "setting to 'something' didn't work")

                provider.send('test.string = ""')
                self.assert_(
                        listen.expect("^test.string = QString:\"\"$"),
                        "setting to empty string didn't work")

                provider.send('unset test.string')
                self.assert_(
                        listen.expect(wantedUnknown("test.string")),
                        "setting to null didn't work")

                listen.wait()
                provider.wait()

class MultipleSubscribers(unittest.TestCase):
        def setUp(self):
                self.flexiprovider = CLTool("context-provide", "--v2","com.nokia.test",
                                            "int","test.int","1",
                                            "string","test.string","foobar",
                                            "double","test.double","2.5",
                                            "truth","test.truth","True")
                self.flexiprovider.expect("Setting key: test.truth") # wait for it
                self.flexiprovider.send("dump")
                self.flexiprovider.expect("Wrote") # wait for it
                self.context_client1 = CLTool("context-listen","test.int","test.double","test.string","test.truth")
                self.context_client2 = CLTool("context-listen","test.int","test.double")
                self.context_client3 = CLTool("context-listen","test.int","test.string","test.truth")
                self.context_client4 = CLTool("context-listen","test.int","test.double","test.string")

        def tearDown(self):
                self.context_client4.wait()
                self.context_client3.wait()
                self.context_client2.wait()
                self.context_client1.wait()
                self.flexiprovider.wait()
                os.unlink('./context-provide.context')

        def testInitialSubscription(self):
                """
                Description
                        4 subscribers instantiate multiple properties of 4 basic data types

                Pre-conditions
                        Provider started via context-provide tool.
                        4 data types supported int, bool, double and string

                Steps
                        Subscribe to the properties, int, bool, double, string
                        Subscribers receive initial values from provider

                Post-conditions
                        Kill provider

                References
                        None
                """

                client1_expected = ["^test.double = double:2.5$",
                                    "^test.int = int:1$",
                                    "^test.string = QString:\"foobar\"$",
                                    "^test.truth = bool:true$"]
                client2_expected = ["^test.double = double:2.5$",
                                    "^test.int = int:1$"]
                client3_expected = ["^test.int = int:1$",
                                    "^test.string = QString:\"foobar\"$",
                                    "^test.truth = bool:true$"]
                client4_expected = ["^test.double = double:2.5$",
                                    "^test.int = int:1$",
                                    "^test.string = QString:\"foobar\"$"]

                self.assert_(self.context_client1.expect(client1_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client2.expect(client2_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client3.expect(client3_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client4.expect(client4_expected), "Actual key values pairs do not match expected")

        def testValueChanged(self):
                """
                Description
                        4 subscribers instantiate multiple properties of 4 basic data types

                Pre-conditions
                        Provider started via context-provide tool.
                        4 data types supported int, bool, double and string

                Steps
                        Subscribe to the properties, int, bool, double, string
                        Values are changed successively from the provider
                        Subscribers receive updated values

                Post-conditions
                        Kill provider

                References
                        None
                """
                client1_expected = ["^test.double = double:2.5$",
                                    "^test.int = int:1$",
                                    "^test.string = QString:\"foobar\"$",
                                    "^test.truth = bool:true$"]
                client2_expected = ["^test.double = double:2.5$",
                                    "^test.int = int:1$"]
                client3_expected = ["^test.int = int:1$",
                                    "^test.string = QString:\"foobar\"$",
                                    "^test.truth = bool:true$"]
                client4_expected = ["^test.double = double:2.5$",
                                    "^test.int = int:1$",
                                    "^test.string = QString:\"foobar\"$"]

                self.assert_(self.context_client1.expect(client1_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client2.expect(client2_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client3.expect(client3_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client4.expect(client4_expected), "Actual key values pairs do not match expected")

                self.flexiprovider.send("test.double = -5.3")
                client1_expected = "^test.double = double:-5.3$"
                client2_expected = "^test.double = double:-5.3$"
                client4_expected = "^test.double = double:-5.3$"

                self.assert_(self.context_client1.expect(client1_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client2.expect(client2_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client4.expect(client4_expected), "Actual key values pairs do not match expected")

                self.context_client3.send("type test.truth")

                client3_expected = "^type: TRUTH$"

                self.assert_(self.context_client3.expect(client3_expected), "Actual key values pairs do not match expected")

                self.flexiprovider.send("unset test.truth")
                client1_expected = wantedUnknown("test.truth")
                client3_expected = wantedUnknown("test.truth")

                self.assert_(self.context_client1.expect(client1_expected), "Actual key values pairs do not match expected")
                self.assert_(self.context_client3.expect(client3_expected), "Actual key values pairs do not match expected")

class MultipleProviders(unittest.TestCase):
        def tearDown(self):
                os.unlink('./context-provide1.context')
                os.unlink('./context-provide2.context')

        def testTwoProviders(self):
                provider1 = CLTool("context-provide", "--v2","com.nokia.test",
                                   "truth","test.truth","True")
                provider1.expect("Setting key: test.truth") # wait for it
                provider1.send("dump context-provide1.context")
                provider1.expect("Wrote") # wait for it

                provider2 = CLTool("context-provide", "--v2","com.nokia.test2",
                                   "int","test.int","24")
                provider2.expect("Setting key: test.int") # wait for it
                provider2.send("dump context-provide2.context")
                provider2.expect("Wrote") # wait for it

                listen = CLTool("context-listen","test.int","test.truth")
                listen.expect("Available commands") # wait for it

                provider2.send("test.int = -68")

                self.assert_(
                        listen.expect("^test.int = int:-68$"))

                provider1.send("test.truth = False")

                self.assert_(
                        listen.expect("^test.truth = bool:false$"))
                listen.wait()
                provider2.wait()
                provider1.wait()

class SubscriptionPause (unittest.TestCase):
        def tearDown(self):
                self.listen.wait()
                self.provider.wait()
                os.unlink('context-provide.context')

        def testPause(self):
                """
                Description
                        A subscriber successively subscribes and unsubscribes from a property

                Pre-conditions
                        Provider started via context-provide tool.
                        2 properties of data type int

                Steps
                        Subscribe to a property
                        Subscriber receives updated value
                        Unsubscribe from the property
                        Subscriber does not receive updated value
                        Resubscribe to the property
                        Subscriber receives updated value

                Post-conditions
                        Kill provider

                References
                        None
                """

                self.provider = CLTool("context-provide", "--v2","com.nokia.test",
                                       "int","test.int","1")
                self.provider.expect("Setting key: test.int") # wait for it
                self.provider.send("dump")
                self.provider.expect("Wrote") # wait for it
                self.listen = CLTool("context-listen","test.int")

                self.assert_(
                        self.listen.expect("^test.int = int:1$"))

                self.provider.send("test.int = -5")
                self.assert_(
                        self.listen.expect("^test.int = int:-5$"))

                self.listen.send("unsubscribe test.int")
                self.provider.send("test.int = 3")
                self.assertFalse(
                        self.listen.expect("^test.int = int:3$", wantdump = False))

                self.listen.send("subscribe test.int")
                self.assert_(
                        self.listen.expect("^test.int = int:3$"))

                self.provider.send("test.int = 6")
                self.assert_(
                        self.listen.expect("^test.int = int:6$"))

        def testWaitForSubscribe(self):
                """
                Description
                Subscriber blocks until WaitForSubsciption returns

                Pre-conditions
                        Provider started via context-provide tool.
                        1 data type supported int

                Steps
                        Delete the property test.int
                        Delay the provider
                        Subscribe to the property test.int
                        Subscriber blocks until the subscription is complete

                Post-conditions
                        Kill provider

                References
                        None
                """
                self.provider = CLTool("context-provide", "--v2","com.nokia.test",
                                       "int","test.int","1")
                self.provider.expect("Setting key: test.int") # wait for it
                self.provider.send("dump")
                self.provider.expect("Wrote") # wait for it
                self.provider.send("sleep 3")
                self.provider.expect("Sleep") # wait for it
                self.listen = CLTool("context-listen")
                self.listen.send("new test.int")
                self.listen.send("waitforsubscription test.int")

                expected = ["^test.int = int:1$",
                            "^wait finished for test.int$"]

                # I don't get it quickly
                self.assertFalse(
                        self.listen.expect(expected, wantdump=False, timeout=1))
                # but get it after a while
                self.assert_(self.listen.expect(expected))

class SubscriptionWaitError (unittest.TestCase):
        def testWaitForSubscribeFail(self):
                """
                Description
                Subscriber blocks until WaitForSubsciption returns, even if provider is not on DBus (specific to ContextKitPlugin)

                Pre-conditions
                        Provider does exist for the property in the registry but it is not running

                Steps
                        Subscribe and wait for the property

                Post-conditions
                        The wait returned in 3 seconds (enough for testing that provider is not on D-Bus)
                """
                context_client = CLTool("context-listen")
                context_client.send("n test.nonexistent")
                context_client.send("w test.nonexistent")

                self.assert_(context_client.expect("wait finished for test.nonexistent"),
                             "Wait for subscription is probably in a dead state")
                context_client.wait()

if __name__ == "__main__":
        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
        unittest.main()
