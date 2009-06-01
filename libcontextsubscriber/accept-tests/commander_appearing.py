#!/usr/bin/env python2.5
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
##   - starts up a client and a provider
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

class CommanderAppearing(unittest.TestCase):
    def startProvider(busname, args):
        ret = Popen(["../../python/context-provide", busname] + args,
              stdin=PIPE,stderr=PIPE,stdout=PIPE)
        # wait for it
        print >>ret.stdin, "info()"
        ret.stdout.readline().rstrip()
        return ret
    startProvider = Callable(startProvider)

    def wanted(name, type, value):
        return "%s = %s:%s" % (name, type, value)
    wanted = Callable(wanted)

    def wantedUnknown(name):
        return "%s is Unknown" % (name)
    wantedUnknown = Callable(wantedUnknown)

    def setUp(self):
        self.flexiprovider = self.startProvider("com.nokia.test",
                                           ["int","test.int","42"])
        self.context_client = Popen(["../cli/context-listen","test.int"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
        self.context_commander = 0

    def tearDown(self):
        os.kill(self.flexiprovider.pid, 9)
        if (self.context_commander != 0): os.kill(self.context_commander.pid, 9)
        os.kill(self.context_client.pid, 9)
        os.unlink('context-provide.context')
        pass

    def testCommanderFunctionality(self):
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.int", "int", "42"),
                         "Initial value is bad")

        self.context_commander = self.startProvider("org.freedesktop.ContextKit.Commander",
                                                    ["int", "test.int", "4242"])
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.int", "int", "4242"),
                         "Value after commander has been started is bad")

        print >>self.context_commander.stdin, "set('test.int', None)"
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wantedUnknown("test.int"),
                         "Value after commander has changed it to unknown is bad")

        print >>self.context_commander.stdin, "set('test.int', 1235)"
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.int", "int", "1235"),
                         "Value after commander has changed it is bad")

        os.kill(self.context_commander.pid, 9)
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.int", "int", "42"),
                         "Value after killing the commander is bad")

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(CommanderAppearing)
    unittest.TextTestRunner(verbosity=2).run(suiteInstallation)

if __name__ == "__main__":
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(10)
    runTests()
