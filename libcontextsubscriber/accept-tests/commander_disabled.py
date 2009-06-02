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
##   - starts up a client with commanding disabled and a provider
##   - starts a commander with a different value for that property
##   - checks for the provider provided value on the client stdout
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

class CommanderDisabled(unittest.TestCase):
    def startProvider(busname, args):
        ret = Popen(["context-provide", busname] + args,
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
        self.context_commander = self.startProvider("org.freedesktop.ContextKit.Commander",
                                                    ["int", "test.int", "4242"])
        os.environ["CONTEXT_CLI_IGNORE_COMMANDER"] = ""
        self.context_client = Popen(["context-listen","test.int"], stdin=PIPE, stdout=PIPE, stderr=PIPE)

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
                         "Provider provided value is wrong")

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(CommanderDisabled)
    unittest.TextTestRunner(verbosity=2).run(suiteInstallation)

if __name__ == "__main__":
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(10)
    runTests()
