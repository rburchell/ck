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
import unittest
import os
import string
from subprocess import Popen, PIPE
from time import sleep

class MemoryPressure(unittest.TestCase):

    def setUp(self):
        self.contextd = Popen("contextd",stdin=PIPE,stderr=PIPE,stdout=PIPE)
        self.context_client = Popen(["context-listen","System.MemoryPressure"],stdin=PIPE,stdout=PIPE,stderr=PIPE)
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
        actual = self.context_client.stdout.readline().rstrip()
        expected = "System.MemoryPressure: int:0" # Normal
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        cmd = 'memload 100MB'
        self.memload = Popen(cmd,stdin=PIPE,stdout=PIPE,stderr=PIPE)
        sleep(2)

        actual = self.context_client.stdout.readline().rstrip()
        expected = "System.MemoryPressure: int:1" # High
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")

        cmd = 'memload 150MB'
        self.memload2 = Popen(cmd,stdin=PIPE,stdout=PIPE,stderr=PIPE)
        sleep(2)

        actual = self.context_client.stdout.readline().rstrip()
        expected = "System.MemoryPressure: int:2" # Critical
        self.assertEqual(actual,expected,"Actual key values pairs do not match expected")
        os.kill(self.memload.pid,9)
        os.kill(self.memload2.pid,9)

def runTests():
    suiteLowMemoryPlugin = unittest.TestLoader().loadTestsFromTestCase(MemoryPressure)
    unittest.TextTestRunner(verbosity=2).run(suiteLowMemoryPlugin)

if __name__ == "__main__":
    runTests()
