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
import string
from subprocess import Popen, PIPE
import time
import signal

def proc_kill(pid):
	os.system('../common/rec-kill.sh %d' % pid)

def timeoutHandler(signum, frame):
    raise Exception('tests has been running for too long')

def stdoutRead (object,lines):
    list = []
    for i in range(lines):
        list.append(object.stdout.readline().rstrip())
    return list

class Subscription(unittest.TestCase):

    def setUp(self):
        self.flexiprovider = Popen(["context-provide","session:com.nokia.test",
               "int","test.int","1",
               "string","test.string","foobar",
               "double","test.double","2.5",
               "truth","test.truth","True"],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)
        os.environ["CONTEXT_PROVIDERS"] = "."

        print >>self.flexiprovider.stdin, "info()"
        self.flexiprovider.stdout.readline()
        self.context_client = Popen(["context-listen","test.int","test.double","test.string","test.truth"],stdin=PIPE,stdout=PIPE,stderr=PIPE)

    def tearDown(self):
        proc_kill(self.flexiprovider.pid)
        proc_kill(self.context_client.pid)
        os.unlink('context-provide.context')

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

        actual = stdoutRead(self.context_client,4)
        actual.sort()
        expected = ["test.double = double:2.5",
                    "test.int = int:1",
                    "test.string = QString:foobar",
                    "test.truth = bool:true"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")
        print >> self.flexiprovider.stdin, "set('test.int',100)"
        self.context_client.stdout.readline().rstrip()
        print >> self.context_client.stdin, "value test.int"
        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["value: int:100"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        print >> self.context_client.stdin, "value test.int defaultValue"
        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["value: int:100"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        print >> self.flexiprovider.stdin, "set('test.int',None)"
        self.context_client.stdout.readline().rstrip()
        print >> self.context_client.stdin, "value test.int"
        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["value is Unknown"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        print >> self.context_client.stdin, "value test.int defaultValue"
        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["value: QString:defaultValue"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        print >> self.context_client.stdin, "key test.int"
        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["key: test.int"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

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

        actual = stdoutRead(self.context_client,4)
        print >> self.context_client.stdin, "ikey test.int"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["ikey: test.int"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")
        print >> self.context_client.stdin, "man test.truth"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["man: A phony but very flexible property."]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")
        print >> self.context_client.stdin, "type test.double"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["type: DOUBLE"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

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

        actual = stdoutRead(self.context_client,4)

        actual.sort()
        expected = ["test.double = double:2.5",
                    "test.int = int:1",
                    "test.string = QString:foobar",
                    "test.truth = bool:true"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

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
        stdoutRead(self.context_client,4)
        print >> self.flexiprovider.stdin, "set('test.truth',False)"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.truth = bool:false"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected %s %s" % (actual, expected))

        print >> self.flexiprovider.stdin, "set('test.truth',None)"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.truth is Unknown"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected %s %s" % (actual, expected))

        print >> self.flexiprovider.stdin, "set('test.truth',True)"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.truth = bool:true"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected %s %s" % (actual, expected))

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
        stdoutRead(self.context_client,4)
        print >> self.flexiprovider.stdin, "set('test.string','')"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.string = QString:"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected %s %s" % (actual, expected))

        print >> self.flexiprovider.stdin, "set('test.string',None)"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.string is Unknown"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected %s %s" % (actual, expected))

class MultipleSubscribers(unittest.TestCase):

    def initActual(self):
        self.client1_actual = stdoutRead(self.context_client1,4)
        self.client1_actual.sort()
        self.client2_actual = stdoutRead(self.context_client2,2)
        self.client2_actual.sort()
        self.client3_actual = stdoutRead(self.context_client3,3)
        self.client3_actual.sort()
        self.client4_actual = stdoutRead(self.context_client4,3)
        self.client4_actual.sort()

    def updateExpected():
        pass

    def setUp(self):
        self.flexiprovider = Popen(["context-provide","session:com.nokia.test",
               "int","test.int","1",
               "string","test.string","foobar",
               "double","test.double","2.5",
               "truth","test.truth","True"],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)
        os.environ["CONTEXT_PROVIDERS"] = "."
        print >>self.flexiprovider.stdin, "info()"
        self.flexiprovider.stdout.readline()

        self.context_client1 = Popen(["context-listen","test.int","test.double","test.string","test.truth"],stdin=PIPE,stdout=PIPE,stderr=PIPE)
        self.context_client2 = Popen(["context-listen","test.int","test.double"],stdin=PIPE,stdout=PIPE,stderr=PIPE)

        self.context_client3 = Popen(["context-listen","test.int","test.string","test.truth"],stdin=PIPE,stdout=PIPE,stderr=PIPE)

        self.context_client4 = Popen(["context-listen","test.int","test.double","test.string"],stdin=PIPE,stdout=PIPE,stderr=PIPE)


    def tearDown(self):
        proc_kill(self.flexiprovider.pid)
        proc_kill(self.context_client1.pid)
        proc_kill(self.context_client2.pid)
        proc_kill(self.context_client3.pid)
        proc_kill(self.context_client4.pid)
        os.unlink('context-provide.context')

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

        self.initActual()
        client1_expected = ["test.double = double:2.5",
                            "test.int = int:1",
                            "test.string = QString:foobar",
                            "test.truth = bool:true"]
        client2_expected = ["test.double = double:2.5",
                            "test.int = int:1"]
        client3_expected = ["test.int = int:1",
                            "test.string = QString:foobar",
                            "test.truth = bool:true"]
        client4_expected = ["test.double = double:2.5",
                            "test.int = int:1",
                            "test.string = QString:foobar"]

        self.assertEqual(self.client1_actual,client1_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client2_actual,client2_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client3_actual,client3_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client4_actual,client4_expected,"Actual key values pairs do not match expected")

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
        self.initActual()
        client1_expected = ["test.double = double:2.5",
                            "test.int = int:1",
                            "test.string = QString:foobar",
                            "test.truth = bool:true"]
        client2_expected = ["test.double = double:2.5",
                            "test.int = int:1"]
        client3_expected = ["test.int = int:1",
                            "test.string = QString:foobar",
                            "test.truth = bool:true"]
        client4_expected = ["test.double = double:2.5",
                            "test.int = int:1",
                            "test.string = QString:foobar"]
        self.assertEqual(self.client1_actual,client1_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client2_actual,client2_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client3_actual,client3_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client4_actual,client4_expected,"Actual key values pairs do not match expected")

        print >> self.flexiprovider.stdin, "set('test.double',-5.3)"

        self.client1_actual = [self.context_client1.stdout.readline().rstrip()]
        self.client1_actual.sort()

        self.client2_actual = [self.context_client2.stdout.readline().rstrip()]
        self.client2_actual.sort()

        self.client4_actual = [self.context_client4.stdout.readline().rstrip()]
        self.client4_actual.sort()

        client1_expected = ["test.double = double:-5.3"]
        client2_expected = ["test.double = double:-5.3"]
        client4_expected = ["test.double = double:-5.3"]

        self.assertEqual(self.client1_actual,client1_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client2_actual,client2_expected,"Actual key values pairs do not match expected")
        self.assertEqual(self.client4_actual,client4_expected,"Actual key values pairs do not match expected")

        print >> self.context_client3.stdin, "type test.truth"

        client3_expected = "type: TRUTH"

        self.client3_actual = self.context_client3.stdout.readline().rstrip()
        self.assertEqual(self.client3_actual,client3_expected,
                         "Actual key values pairs do not match expected %s %s " % (self.client3_actual,client3_expected))


        print >> self.flexiprovider.stdin, "set('test.truth',None)"

        self.client1_actual = [self.context_client1.stdout.readline().rstrip()]
        self.client1_actual.sort()

        self.client3_actual = [self.context_client3.stdout.readline().rstrip()]
        self.client3_actual.sort()

        client1_expected = ["test.truth is Unknown"]
        client3_expected = ["test.truth is Unknown"]

        self.assertEqual(self.client1_actual,client1_expected,
                         "Actual key values pairs do not match expected %s %s " % (self.client1_actual,client1_expected))
        self.assertEqual(self.client3_actual,client3_expected,
                         "Actual key values pairs do not match expected %s %s " % (self.client3_actual,client3_expected))

class MultipleProviders(unittest.TestCase):

    def setUp(self):
        os.environ["CONTEXT_PROVIDE_REGISTRY_FILE"] = "./context-provide1.context"
        self.flexiprovider1 = Popen(["context-provide","session:com.nokia.test",
               "truth","test.truth","True"],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)

        os.environ["CONTEXT_PROVIDE_REGISTRY_FILE"] = "./context-provide2.context"
        self.flexiprovider2 = Popen(["context-provide","session:com.nokia.test2",
               "int","test.int","24"],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)

        os.environ["CONTEXT_PROVIDERS"] = "."

        print >> self.flexiprovider1.stdin, "info()"
        self.flexiprovider1.stdout.readline()

        self.context_client = Popen(["context-listen","test.int","test.truth"]
                                    ,stdin=PIPE,stdout=PIPE,stderr=PIPE)

    def tearDown(self):
        proc_kill(self.context_client.pid)
        proc_kill(self.flexiprovider1.pid)
        proc_kill(self.flexiprovider2.pid)
        os.unlink('context-provide1.context')
        os.unlink('context-provide2.context')

    def testTwoProviders(self):
        stdoutRead(self.context_client,2)
        print >>self.flexiprovider2.stdin, "set('test.int',-68)"

        actual = self.context_client.stdout.readline().rstrip()
        expected = "test.int = int:-68"
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        print >>self.flexiprovider1.stdin, "set('test.truth',False)"

        actual = self.context_client.stdout.readline().rstrip()
        expected = "test.truth = bool:false"
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")


class SubscriptionPause (unittest.TestCase):

    def setUp(self):
        os.environ["CONTEXT_PROVIDE_REGISTRY_FILE"] = "./context-provide.context"
        self.flexiprovider = Popen(["context-provide","session:com.nokia.test",
               "int","test.int","1",
               "int","test.int.second","-1" ],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)

        os.environ["CONTEXT_PROVIDERS"] = "."

        print >> self.flexiprovider.stdin, "info()"
        self.flexiprovider.stdout.readline()

        self.context_client = Popen(["context-listen","test.int"],stdin=PIPE,stdout=PIPE,stderr=PIPE)

    def tearDown(self):
        proc_kill(self.flexiprovider.pid)
        proc_kill(self.context_client.pid)
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

        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["test.int = int:1"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")
        print >> self.flexiprovider.stdin, "set('test.int',-5)"
        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["test.int = int:-5"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        print >> self.context_client.stdin, "unsubscribe test.int"
        print >> self.flexiprovider.stdin, "set('test.int',3)"

        print >> self.context_client.stdin, "subscribe test.int"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.int = int:3"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")
        print >> self.flexiprovider.stdin, "set('test.int',6)"
        actual = [self.context_client.stdout.readline().rstrip()]

        expected = ["test.int = int:6"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")


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
        print >> self.context_client.stdin, "delete test.int"
        print >>self.flexiprovider.stdin, "import time ; time.sleep(3)"

        print >> self.context_client.stdin, "new test.int"
        print >> self.context_client.stdin, "waitforsubscription test.int"


        expected = ["test.int = int:1"]
        actual = [self.context_client.stdout.readline().rstrip()]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        expected = ["wait finished for test.int"]
        actual = [self.context_client.stdout.readline().rstrip()]

        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")


def runTests():
    suiteSubscription = unittest.TestLoader().loadTestsFromTestCase(Subscription)
    suiteSubscribers = unittest.TestLoader().loadTestsFromTestCase(MultipleSubscribers)
    suiteProviders = unittest.TestLoader().loadTestsFromTestCase(MultipleProviders)
    suitePause = unittest.TestLoader().loadTestsFromTestCase(SubscriptionPause)

    errors = []
    result = unittest.TextTestRunner(verbosity=2).run(suiteSubscription)
    errors += result.errors + result.failures
    result = unittest.TextTestRunner(verbosity=2).run(suiteSubscribers)
    errors += result.errors + result.failures
    result = unittest.TextTestRunner(verbosity=2).run(suiteProviders)
    errors += result.errors + result.failures
    result = unittest.TextTestRunner(verbosity=2).run(suitePause)
    errors += result.errors + result.failures

    return len(errors)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(30)
    sys.exit(runTests())
