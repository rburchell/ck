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
        self.context_client = Popen(["../cli/context-cli","test.int","test.double","test.string","test.truth"],stdout=PIPE,stderr=PIPE)

    def tearDown(self):
        os.kill(self.flexiprovider.pid,9)
        os.kill(self.context_client.pid,9)

    def testSimpleProperty(self):
        got = [self.context_client.stdout.readline().rstrip(),
               self.context_client.stdout.readline().rstrip(),
               self.context_client.stdout.readline().rstrip(),
               self.context_client.stdout.readline().rstrip()]
        got.sort()
        wanted = ["test.double = double:2.5",
                  "test.int = int:1",
                  "test.string = QString:foobar",
                  "test.truth = bool:true",]
        self.assertEqual(got,wanted,"Properties gotten do not match expected")

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(Subscription)

    unittest.TextTestRunner(verbosity=2).run(suiteInstallation)

if __name__ == "__main__":
    runTests()
