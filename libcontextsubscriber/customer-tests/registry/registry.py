#!/usr/bin/env python
##
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
## This test:
##   - prints out the information that is present in the registry for a property
##   - checks for the provided values on the client stdout
##   - starts a commander with a different value for that property
##   - checks the output for the changed value
##   - changes the value inside the commander to unknown, check it
##   - changes the value inside the commander back to some meaningful value, check it
##   - kills the commander
##   - checks that value goes back to normal
##

import sys
import os
import signal

import unittest
from subprocess import Popen, PIPE

def timeoutHandler(signum, frame):
    raise Exception('tests has been running for too long')

class Callable:
    def __init__(self, anycallable):
        self.__call__ = anycallable

def startProvider(busname, args):
    ret = Popen(["context-provide", busname] + args,
                stdin=PIPE,stderr=PIPE,stdout=PIPE)
    # wait for it
    print >>ret.stdin, "info()"
    ret.stdout.readline().rstrip()
    return ret
startProvider = Callable(startProvider)

class PrintInfoRunning(unittest.TestCase):
    def setUp(self):
        pass
    def tearDown(self):
        os.kill(self.flexiprovider.pid, 9)
        os.unlink('context-provide.context')

    def testReturnValue(self):
        self.flexiprovider = startProvider("com.nokia.test",
                                           ["int", "test.int", "-5", 
                                            "string", "test.string", "something",
                                            "double", "test.double", "4.231",
                                            "truth", "test.truth", "False"])
        self.info_client = Popen(["context-print-info","test.int", "test.string", "test.double", "test.truth", "test.nothing"], stdin=PIPE, stdout=PIPE, stderr=PIPE)

        returnValue = self.info_client.wait()
        self.assertEqual(returnValue, 0, "context-print-info exited with return value != 0")

class PrintingProperties(unittest.TestCase):
    def setUp(self):
        self.flexiprovider = startProvider("com.nokia.test",
                                           ["int", "test.int", "-5", 
                                            "string", "test.string", "something",
                                            "double", "test.double", "4.231",
                                            "truth", "test.truth", "False"])
        self.info_client = Popen(["context-print-info","test.int", "test.string", "test.double", "test.truth", "test.nothing"], stdin=PIPE, stdout=PIPE, stderr=PIPE)

    def tearDown(self):
        os.kill(self.flexiprovider.pid, 9)
        os.kill(self.info_client.pid, 9)
        os.unlink('context-provide.context')
        pass

    def testProperties(self):
        expected_results = ["Key: test.int", 
                            "Existence: true", 
                            "Provider DBus type: session",
                            "Provider DBus name: com.nokia.test",
                            "Documentation: A phony but very flexible property.",
                            "----------",
                            "Key: test.string",
                            "Existence: true",
                            "Provider DBus type: session",
                            "Provider DBus name: com.nokia.test",
                            "Documentation: A phony but very flexible property.",
                            "----------",
                            "Key: test.double",
                            "Existence: true",
                            "Provider DBus type: session",
                            "Provider DBus name: com.nokia.test",
                            "Documentation: A phony but very flexible property.",
                            "----------",
                            "Key: test.truth",
                            "Existence: true",
                            "Provider DBus type: session",
                            "Provider DBus name: com.nokia.test",
                            "Documentation: A phony but very flexible property.",
                            "----------",
                            "Key: test.nothing",
                            "Existence: false",
                            "----------"]

        for i in range(len(expected_results)):
            got = self.info_client.stdout.readline().rstrip()
            self.assertEqual(got, expected_results[i].rstrip())

def runTests():
    suitePrintInfoRunning = unittest.TestLoader().loadTestsFromTestCase(PrintInfoRunning)
    suiteProperties = unittest.TestLoader().loadTestsFromTestCase(PrintingProperties)

    errors = []
    result = unittest.TextTestRunner(verbosity=2).run(suitePrintInfoRunning) 
    errors += result.errors + result.failures
    result = unittest.TextTestRunner(verbosity=2).run(suiteProperties)
    errors += result.errors + result.failures

    return len(errors)

if __name__ == "__main__":
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(10)
    sys.exit(runTests())
