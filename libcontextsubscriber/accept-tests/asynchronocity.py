#!/usr/bin/env python2.5
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
##   - starts up two providers (with one properties each)
##   - issues a sleep 3 in one of them
##   - subscribes to both properties/providers
##   - reads the value from the non-sleeping one
##   - reads the value from the sleeping one
##   - the time difference between the two reads must be 2 seconds at least

import sys
import os
import signal
import re
import time

import unittest
from subprocess import Popen, PIPE

def timeoutHandler(signum, frame):
    raise Exception('tests has been running for too long')

class Callable:
    def __init__(self, anycallable):
        self.__call__ = anycallable

class Asynchronous(unittest.TestCase):
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

    def testCommanderFunctionality(self):
        ### SETUP
        os.environ["CONTEXT_FLEXI_XML"] = "./flexi-properties-slow.xml"
        self.flexiprovider_slow = self.startProvider("com.nokia.slow",
                                                     ["int","test.slow","42"])
        os.environ["CONTEXT_FLEXI_XML"] = "./flexi-properties-fast.xml"
        self.flexiprovider_fast = self.startProvider("com.nokia.fast",
                                                     ["int","test.fast","42"])
        print >>self.flexiprovider_slow.stdin, "import time ; time.sleep(3)"
        self.context_client = Popen(["../cli/context-listen","test.fast", "test.slow"], stdin=PIPE, stdout=PIPE, stderr=PIPE)

        # check the fast property
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.fast", "int", "42"),
                         "Bad value for the fast property")
        fast_time = time.time()

        # check the slow property
        got = self.context_client.stdout.readline().rstrip()
        self.assertEqual(got,
                         self.wanted("test.slow", "int", "42"),
                         "Bad value for the slow property")
        slow_time = time.time()

        self.assert_(slow_time - fast_time > 2.0,
                     "The arrival time of the fast and slow property is not far enough from each other")

        ### TEARDOWN
        os.kill(self.flexiprovider_fast.pid, 9)
        os.kill(self.flexiprovider_slow.pid, 9)
        os.unlink('flexi-properties-slow.xml')
        os.unlink('flexi-properties-fast.xml')

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(Asynchronous)
    unittest.TextTestRunner(verbosity=2).run(suiteInstallation)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(10)
    runTests()
