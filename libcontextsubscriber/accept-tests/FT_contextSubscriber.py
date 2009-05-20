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
import sys
sys.path.append("../../python")
import unittest
import os
import string
from subprocess import Popen, PIPE

class Subscription(unittest.TestCase):

    def setUp(self):
        self.flexiprovider = Popen(["../../python/context-provide","session:com.nokia.test",
               "int","test.int","1",
               "string","test.string","foobar",
               "double","test.double","2.5",
               "truth","test.truth","true"],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)
        os.environ["CONTEXT_PROVIDERS"] = "."
	print >>self.flexiprovider.stdin, "info()"
        self.flexiprovider.stdout.readline()
        self.context_client = Popen(["../cli/context-listener","test.int","test.double","test.string","test.truth"],stdout=PIPE,stderr=PIPE)

    def tearDown(self):
        os.kill(self.flexiprovider.pid,9)
        os.kill(self.context_client.pid,9)
	os.unlink('flexi-properties.xml')

    def testAllDataTypes(self):
        actual = [self.context_client.stdout.readline().rstrip(),
                  self.context_client.stdout.readline().rstrip(),
                  self.context_client.stdout.readline().rstrip(),
                  self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["test.double = double:2.5",
                    "test.int = int:1",
                    "test.string = QString:foobar",
                    "test.truth = bool:true"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

    def testTruthPermutations(self):
        self.context_client.stdout.readline().rstrip()
        self.context_client.stdout.readline().rstrip()
        self.context_client.stdout.readline().rstrip()
        self.context_client.stdout.readline().rstrip()
        print >> self.flexiprovider.stdin, "set('test.truth',False)"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.truth = bool:false"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected %s %s" % (actual, expected))

        print >> self.flexiprovider.stdin, "set('test.truth',None)"
        actual = [self.context_client.stdout.readline().rstrip()]
        expected = ["test.truth is Unknown"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected %s %s" % (actual, expected))

    def testStringPermutations(self):
        self.context_client.stdout.readline().rstrip()
        self.context_client.stdout.readline().rstrip()
        self.context_client.stdout.readline().rstrip()
        self.context_client.stdout.readline().rstrip()
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
        self.client1_actual = [self.context_client1.stdout.readline().rstrip(),
                               self.context_client1.stdout.readline().rstrip(),
                               self.context_client1.stdout.readline().rstrip(),
                               self.context_client1.stdout.readline().rstrip()]
        self.client1_actual.sort()

        self.client2_actual = [self.context_client2.stdout.readline().rstrip(),
                               self.context_client2.stdout.readline().rstrip()]
        self.client2_actual.sort()
        self.client3_actual = [self.context_client3.stdout.readline().rstrip(),
                               self.context_client3.stdout.readline().rstrip(),
                               self.context_client3.stdout.readline().rstrip()]
        self.client3_actual.sort()
        self.client4_actual = [self.context_client4.stdout.readline().rstrip(),
                               self.context_client4.stdout.readline().rstrip(),
                               self.context_client4.stdout.readline().rstrip()]
        self.client4_actual.sort()

    def updateExpected():
        pass

    def setUp(self):
        self.flexiprovider = Popen(["../../python/context-provide","session:com.nokia.test",
               "int","test.int","1",
               "string","test.string","foobar",
               "double","test.double","2.5",
               "truth","test.truth","true"],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)
        os.environ["CONTEXT_PROVIDERS"] = "."
	print >>self.flexiprovider.stdin, "info()"
        self.flexiprovider.stdout.readline()

        self.context_client1 = Popen(["../cli/context-listener","test.int","test.double","test.string","test.truth"],stdout=PIPE,stderr=PIPE)
        self.context_client2 = Popen(["../cli/context-listener","test.int","test.double"],stdout=PIPE,stderr=PIPE)

        self.context_client3 = Popen(["../cli/context-listener","test.int","test.string","test.truth"],stdout=PIPE,stderr=PIPE)

        self.context_client4 = Popen(["../cli/context-listener","test.int","test.double","test.string"],stdout=PIPE,stderr=PIPE)


    def tearDown(self):
        os.kill(self.flexiprovider.pid,9)
        os.kill(self.context_client1.pid,9)
        os.kill(self.context_client2.pid,9)
        os.kill(self.context_client3.pid,9)
        os.kill(self.context_client4.pid,9)
	os.unlink('flexi-properties.xml')

    def testInitialSubscription(self):
        """ """
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
        """ """
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
        os.environ["CONTEXT_FLEXI_XML"] = "./flexi-provider1.xml"
        self.flexiprovider1 = Popen(["../../python/context-provide","session:com.nokia.test",
               "int","test.int","1",
               "string","test.string","foobar",
               "double","test.double","2.5",
               "truth","test.truth","true"],
              stdin=PIPE,stderr=PIPE,stdout=PIPE)

        os.environ["CONTEXT_PROVIDERS"] = "."

	print >> self.flexiprovider1.stdin, "info()"
        self.flexiprovider1.stdout.readline()

        self.context_client = Popen(["../cli/context-listener","test.int","test.double","test.string","test.truth"],stdout=PIPE,stderr=PIPE)

    def tearDown(self):
        os.kill(self.flexiprovider1.pid,9)
        os.kill(self.flexiprovider2.pid,9)
        os.kill(self.context_client.pid,9)
	os.unlink('flexi-provider1.xml')
	os.unlink('flexi-provider2.xml')

    def testIdenticalProvider(self):
        """ """
        os.environ["CONTEXT_FLEXI_XML"] = "./flexi-provider2.xml"
        self.flexiprovider2 = Popen(["../../python/context-provide","session:com.nokia.test2",
                                     "int","test.int","1",
                                     "string","test.string","foobar",
                                     "double","test.double","2.5",
                                     "truth","test.truth","true"],
                                    stdin=PIPE,stderr=PIPE,stdout=PIPE)
        print >>self.flexiprovider2.stdin, "info()"
        self.flexiprovider2.stdout.readline()
        self.flexiprovider2.stdout.readline()
        self.flexiprovider2.stdout.readline()


def runTests():
    suiteSubscription = unittest.TestLoader().loadTestsFromTestCase(Subscription)
    suiteSubscribers = unittest.TestLoader().loadTestsFromTestCase(MultipleSubscribers)
    suiteProviders = unittest.TestLoader().loadTestsFromTestCase(MultipleProviders)

    unittest.TextTestRunner(verbosity=2).run(suiteSubscription)
    unittest.TextTestRunner(verbosity=2).run(suiteSubscribers)
    unittest.TextTestRunner(verbosity=2).run(suiteProviders)

if __name__ == "__main__":
    runTests()
