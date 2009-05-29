#!/usr/bin/env python
##
## @file FT_memoryPressure.py
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
import time

class MemoryPressure(unittest.TestCase):

    def setUp(self):
        self.contextd = Popen("contextd",stdin=PIPE,stderr=PIPE,stdout=PIPE)
        self.context_client = Popen(["../../libcontextsubscriber/cli/context-listener","System.MemoryPressure"],stdin=PIPE,stdout=PIPE,stderr=PIPE)
        cmd = 'echo 15000 > /proc/sys/vm/lowmem_notify_high_pages'
        os.system(cmd)

    def tearDown(self):
        os.kill(self.contextd.pid,9)
        os.kill(self.context_client.pid,9)

    def testMemoryPressure(self):
        """
        Description

        Pre-conditions

        Steps

        Post-conditions

        References

        """
        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["value: int:0"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        cmd = 'memload 100MB'
        os.system(cmd)

        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["value: int:1"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        cmd = 'memload 150MB'
        os.system(cmd)

        actual = [self.context_client.stdout.readline().rstrip()]
        actual.sort()
        expected = ["value: int:2"]
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

def runTests():
    suiteLowMemoryPlugin = unittest.TestLoader().loadTestsFromTestCase(MemoryPressure)
    unittest.TextTestRunner(verbosity=2).run(suiteLowMemoryPlugin)

if __name__ == "__main__":
    runTests()
