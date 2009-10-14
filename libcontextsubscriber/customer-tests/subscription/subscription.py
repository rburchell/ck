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
from ContextKit.cltool import CLTool

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
                provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth")
                self.assert_(
                        listen.expectAll(CLTool.STDOUT,
                                         ["\ntest.double = double:2.5\n",
                                          "\ntest.int = int:1\n",
                                          "\ntest.string = QString:foobar\n",
                                          "\ntest.truth = bool:true\n"],
                                         1),
                        "Actual key values pairs do not match expected")

                provider.send("test.int = 100")
                listen.expect(CLTool.STDOUT, "int:100", 10) # wait for it
                listen.send("value test.int")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nvalue: int:100\n",
                                      1),
                        "Value command returned wrong value")

                listen.send("value test.int defaultValue")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nvalue: int:100\n",
                                      1),
                        "Value command returned wrong value")

                provider.send("unset test.int")
                listen.expect(CLTool.STDOUT, "Unknown", 10) # wait for it
                listen.send("value test.int")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nvalue is Unknown\n",
                                      1),
                        "Value command returned wrong value")

                listen.send("value test.int defaultValue")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nvalue: QString:defaultValue\n",
                                      1),
                        "Value command returned wrong value")

                listen.send("key test.int")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nkey: test.int\n",
                                      1),
                        "Key command returned wrong value")

                provider.kill()
                listen.kill()

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
                provider.expect(CLTool.STDOUT, "Setting key: test.truth", 10) # wait for it
                provider.send("dump")
                provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth")
                listen.expectAll(CLTool.STDOUT,
                                 ["\ntest.double = double:2.5\n",
                                  "\ntest.int = int:1\n",
                                  "\ntest.string = QString:foobar\n",
                                  "\ntest.truth = bool:true\n"],
                                 10) # wait for it
                listen.send("ikey test.int")

                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nikey: test.int\n",
                                      1),
                        "ikey didn't work")

                listen.send("man test.truth")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nman: A phony but very flexible property.\n",
                                      1),
                        "man didn't work")

                listen.send("type test.double")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntype: DOUBLE\n",
                                      1),
                        "type didn't work")
                provider.kill()
                listen.kill()

        def testTypes(self):
                provider = CLTool("context-provide", "--v2", "com.nokia.test",
                                 "int","test.int","1",
                                 "string","test.string","foobar",
                                 "double","test.double","2.5",
                                 "truth","test.truth","True")
                provider.expect(CLTool.STDOUT, "Setting key: test.truth", 10) # wait for it
                provider.send("dump")
                provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth", "test.fake")
                listen.expectAll(CLTool.STDOUT,
                                 ["\ntest.double = double:2.5\n",
                                  "\ntest.int = int:1\n",
                                  "\ntest.string = QString:foobar\n",
                                  "\ntest.truth = bool:true\n"],
                                 10) # wait for it

		# test querying the type of all properties
                listen.send("type test.int")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntype: INT\n",
                                      1))

                listen.send("type test.double")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntype: DOUBLE\n",
                                      1))

                listen.send("type test.truth")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntype: TRUTH\n",
                                      1))

                listen.send("type test.string")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntype: STRING\n",
                                      1))

                listen.send("type test.fake")
                self.assert_(
			listen.expect(CLTool.STDOUT,
				      "\ntype:\n",
				      1))

                provider.kill()
                listen.kill()

        def testProviders(self):
                provider = CLTool("context-provide", "--v2", "com.nokia.test",
                                 "int","test.int","1",
                                 "string","test.string","foobar",
                                 "double","test.double","2.5",
                                 "truth","test.truth","True")
                provider.expect(CLTool.STDOUT, "Setting key: test.truth", 10) # wait for it
                provider.send("dump")
                provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth", "test.fake")
                listen.expectAll(CLTool.STDOUT,
                                 ["\ntest.double = double:2.5\n",
                                  "\ntest.int = int:1\n",
                                  "\ntest.string = QString:foobar\n",
                                  "\ntest.truth = bool:true\n"],
                                 10) # wait for it

		# test querying the provider(s)
                listen.send("providers test.int")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nproviders: contextkit-dbus/session:com.nokia.test\n",
                                      1))

                listen.send("providers test.fake")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\nproviders:\n",
                                      1))
                provider.kill()
                listen.kill()

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
                provider.expect(CLTool.STDOUT, "Setting key: test.truth", 10) # wait for it
                provider.send("dump")
                provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                listen = CLTool("context-listen", "test.double", "test.string", "test.int", "test.truth")
                self.assert_(
                        listen.expectAll(CLTool.STDOUT,
                                         ["\ntest.double = double:2.5\n",
                                          "\ntest.int = int:1\n",
                                          "\ntest.string = QString:foobar\n",
                                          "\ntest.truth = bool:true\n"],
                                         10),
                        "Actual key values pairs do not match expected")
                provider.kill()
                listen.kill()

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
                provider.expect(CLTool.STDOUT, "Setting key: test.truth", 10) # wait for it
                provider.send("dump")
                provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                provider.send("test.truth = False")
                provider.expect(CLTool.STDOUT, "Setting", 10) # wait for it
                listen = CLTool("context-listen", "test.truth")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.truth = bool:false\n",
                                      3),
                        "setting to false didn't work")

                provider.send("unset test.truth")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.truth is Unknown\n",
                                      1),
                        "setting to unknown didn't work")

                provider.send("test.truth = True")
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.truth = bool:true\n",
                                      1),
                        "setting to true didn't work")
                provider.kill()
                listen.kill()

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
                provider.expect(CLTool.STDOUT, "Setting key: test.string", 10) # wait for it
                provider.send("dump")
                provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                listen = CLTool("context-listen", "test.string")

                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.string = QString:something\n",
                                      1),
                        "setting to 'something' didn't work")

                provider.send('test.string = ""')
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.string = QString:\n",
                                      1),
                        "setting to empty string didn't work")

                provider.send('unset test.string')
                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.string is Unknown\n",
                                      1),
                        "setting to null didn't work")

                provider.kill()
                listen.kill()

class MultipleSubscribers(unittest.TestCase):
        def setUp(self):
                self.flexiprovider = CLTool("context-provide", "--v2","com.nokia.test",
                                            "int","test.int","1",
                                            "string","test.string","foobar",
                                            "double","test.double","2.5",
                                            "truth","test.truth","True")
                self.flexiprovider.expect(CLTool.STDOUT, "Setting key: test.truth", 10) # wait for it
                self.flexiprovider.send("dump")
                self.flexiprovider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                self.context_client1 = CLTool("context-listen","test.int","test.double","test.string","test.truth")
                self.context_client2 = CLTool("context-listen","test.int","test.double")
                self.context_client3 = CLTool("context-listen","test.int","test.string","test.truth")
                self.context_client4 = CLTool("context-listen","test.int","test.double","test.string")
                self.context_client1.expect(CLTool.STDERR, "Available commands", 10) # wait for it
                self.context_client2.expect(CLTool.STDERR, "Available commands", 10) # wait for it
                self.context_client3.expect(CLTool.STDERR, "Available commands", 10) # wait for it
                self.context_client4.expect(CLTool.STDERR, "Available commands", 10) # wait for it

        def tearDown(self):
                self.flexiprovider.kill()
                self.context_client1.kill()
                self.context_client2.kill()
                self.context_client3.kill()
                self.context_client4.kill()
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

                client1_expected = ["\ntest.double = double:2.5\n",
                                    "\ntest.int = int:1\n",
                                    "\ntest.string = QString:foobar\n",
                                    "\ntest.truth = bool:true\n"]
                client2_expected = ["\ntest.double = double:2.5\n",
                                    "\ntest.int = int:1\n"]
                client3_expected = ["\ntest.int = int:1\n",
                                    "\ntest.string = QString:foobar\n",
                                    "\ntest.truth = bool:true\n"]
                client4_expected = ["\ntest.double = double:2.5\n",
                                    "\ntest.int = int:1\n",
                                    "\ntest.string = QString:foobar\n"]

                self.assert_(self.context_client1.expectAll(CLTool.STDOUT, client1_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client2.expectAll(CLTool.STDOUT, client2_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client3.expectAll(CLTool.STDOUT, client3_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client4.expectAll(CLTool.STDOUT, client4_expected, 1), "Actual key values pairs do not match expected")

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
                client1_expected = ["\ntest.double = double:2.5\n",
                                    "\ntest.int = int:1\n",
                                    "\ntest.string = QString:foobar\n",
                                    "\ntest.truth = bool:true\n"]
                client2_expected = ["\ntest.double = double:2.5\n",
                                    "\ntest.int = int:1\n"]
                client3_expected = ["\ntest.int = int:1\n",
                                    "\ntest.string = QString:foobar\n",
                                    "\ntest.truth = bool:true\n"]
                client4_expected = ["\ntest.double = double:2.5\n",
                                    "\ntest.int = int:1\n",
                                    "\ntest.string = QString:foobar\n"]

                self.assert_(self.context_client1.expectAll(CLTool.STDOUT, client1_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client2.expectAll(CLTool.STDOUT, client2_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client3.expectAll(CLTool.STDOUT, client3_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client4.expectAll(CLTool.STDOUT, client4_expected, 1), "Actual key values pairs do not match expected")

                self.flexiprovider.send("test.double = -5.3")
                client1_expected = "\ntest.double = double:-5.3\n"
                client2_expected = "\ntest.double = double:-5.3\n"
                client4_expected = "\ntest.double = double:-5.3\n"

                self.assert_(self.context_client1.expect(CLTool.STDOUT, client1_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client2.expect(CLTool.STDOUT, client2_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client4.expect(CLTool.STDOUT, client4_expected, 1), "Actual key values pairs do not match expected")

                self.context_client3.send("type test.truth")

                client3_expected = "\ntype: TRUTH\n"

                self.assert_(self.context_client3.expect(CLTool.STDOUT, client3_expected, 1), "Actual key values pairs do not match expected")

                self.flexiprovider.send("unset test.truth")
                client1_expected = "\ntest.truth is Unknown\n"
                client3_expected = "\ntest.truth is Unknown\n"

                self.assert_(self.context_client1.expect(CLTool.STDOUT, client1_expected, 1), "Actual key values pairs do not match expected")
                self.assert_(self.context_client3.expect(CLTool.STDOUT, client3_expected, 1), "Actual key values pairs do not match expected")

class MultipleProviders(unittest.TestCase):
        def tearDown(self):
                os.unlink('./context-provide1.context')
                os.unlink('./context-provide2.context')

        def testTwoProviders(self):
                provider1 = CLTool("context-provide", "--v2","com.nokia.test",
                                   "truth","test.truth","True")
                provider1.expect(CLTool.STDOUT, "Setting key: test.truth", 10) # wait for it
                provider1.send("dump context-provide1.context")
                provider1.expect(CLTool.STDOUT, "Wrote", 10) # wait for it

                provider2 = CLTool("context-provide", "--v2","com.nokia.test2",
                                   "int","test.int","24")
                provider2.expect(CLTool.STDOUT, "Setting key: test.int", 10) # wait for it
                provider2.send("dump context-provide2.context")
                provider2.expect(CLTool.STDOUT, "Wrote", 10) # wait for it

                listen = CLTool("context-listen","test.int","test.truth")

                provider2.send("test.int = -68")

                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.int = int:-68\n",
                                      1))

                provider1.send("test.truth = False")

                self.assert_(
                        listen.expect(CLTool.STDOUT,
                                      "\ntest.truth = bool:false\n",
                                      1))
                provider1.kill()
                provider2.kill()
                listen.kill()

class SubscriptionPause (unittest.TestCase):
        def tearDown(self):
                self.provider.kill()
                self.listen.kill()
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
                self.provider.expect(CLTool.STDOUT, "Setting key: test.int", 10) # wait for it
                self.provider.send("dump")
                self.provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                self.listen = CLTool("context-listen","test.int")

                self.assert_(
                        self.listen.expect(CLTool.STDOUT,
                                           "\ntest.int = int:1\n",
                                           1))

                self.provider.send("test.int = -5")
                self.assert_(
                        self.listen.expect(CLTool.STDOUT,
                                           "\ntest.int = int:-5\n",
                                           1))

                self.listen.send("unsubscribe test.int")
                self.provider.send("test.int = 3")
                self.assertFalse(
                        self.listen.expect(CLTool.STDOUT,
                                           "\ntest.int = int:3\n",
                                           1,
                                           False))

                self.listen.send("subscribe test.int")
                self.assert_(
                        self.listen.expect(CLTool.STDOUT,
                                           "\ntest.int = int:3\n",
                                           1))

                self.provider.send("test.int = 6")
                self.assert_(
                        self.listen.expect(CLTool.STDOUT,
                                           "\ntest.int = int:6\n",
                                           1))

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
                self.provider.expect(CLTool.STDOUT, "Setting key: test.int", 10) # wait for it
                self.provider.send("dump")
                self.provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
                self.provider.send("sleep 3")
                self.provider.expect(CLTool.STDOUT, "Sleep", 10) # wait for it
                self.listen = CLTool("context-listen")
                self.listen.send("new test.int")
                self.listen.send("waitforsubscription test.int")

                expected = ["\ntest.int = int:1\n",
                            "\nwait finished for test.int\n"]

                # I don't get it quickly
                self.assertFalse(
                        self.listen.expectAll(CLTool.STDOUT,
                                              expected,
                                              1,
                                              False))
                # but get it after a while
                self.assert_(self.listen.expectAll(CLTool.STDOUT, expected, 5))

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

                self.assert_(context_client.expect(CLTool.STDOUT,
                                                   "wait finished for test.nonexistent",
                                                   3),
                             "Wait for subscription is probably in a dead state")
                context_client.kill()

if __name__ == "__main__":
        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
        unittest.main()
