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
##   - starts a commander with a different value and type for a provider provided property
##                    and with a new string non-provided property
##   - checks that the client gets an error for the provider provided property
##   - checks that the client gets the new string non-provided property
##   - changes the non-provided property's type to int and checks it
##     (so this proves that type checks are always off for commander added properties)

import sys
import os
import signal
import re

import unittest
from subprocess import Popen, PIPE

def timeoutHandler(signum, frame):
    raise Exception('tests has been running for too long')

class Callable:
    def __init__(self, anycallable):
        self.__call__ = anycallable

class CommanderNonExistent(unittest.TestCase):
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
        self.context_commander = self.startProvider("org.freedesktop.ContextKit.Commander",
                                                    ["string", "test.int", "foobar",
                                                     "string", "test.string", "barfoo"])
        self.context_client = Popen(["../cli/context-listener","test.int", "test.string"], stdin=PIPE, stdout=PIPE, stderr=PIPE)

    def tearDown(self):
        os.kill(self.flexiprovider.pid, 9)
        if (self.context_commander != 0): os.kill(self.context_commander.pid, 9)
        os.kill(self.context_client.pid, 9)
        os.unlink('flexi-properties.xml')

    def testCommanderFunctionality(self):
        line = self.context_client.stderr.readline().rstrip()
        while not line.startswith('(PROVIDER) ERROR: bad type for "test.int" wanted: "INT" got: QString'):
            line = self.context_client.stderr.readline().rstrip()
        # if we are here, then the type check worked

        # check the non-existent property
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.string", "QString", "barfoo"),
                         "Non-existent property couldn't be commanded")

        # change the type of the non-existent property
        print >>self.context_commander.stdin, "add(INT('test.string', 42))"
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.string", "int", "42"),
                         "Non-existent property's type couldn't be overwritten")

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(CommanderNonExistent)
    unittest.TextTestRunner(verbosity=2).run(suiteInstallation)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(10)
    runTests()
