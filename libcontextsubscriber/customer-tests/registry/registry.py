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

import sys
import os
import unittest
from ContextKit.cltool import CLTool

class PrintInfoRunning(unittest.TestCase):
    def tearDown(self):
        os.unlink('context-provide.context')

    def testReturnValue(self):
        provider = CLTool("context-provide", "--v2", "com.nokia.test",
                          "int", "test.int", "-5",
                          "string", "test.string", "something",
                          "double", "test.double", "4.231",
                          "truth", "test.truth", "False")
        provider.send("dump")
        self.assert_(provider.expect("Wrote ./context-provide.context")) # wait for it
        info_client = CLTool("context-ls","test.*")

        returnValue = info_client.wait()
        self.assertEqual(returnValue, 0, "context-ls exited with return value != 0")
        provider.close()

class PrintingProperties(unittest.TestCase):
    def tearDown(self):
        os.unlink('context-provide.context')

    def testProperties(self):
        provider = CLTool("context-provide", "--v2", "com.nokia.test",
                          "int", "test.int", "-5",
                          "string", "test.string", "something",
                          "double", "test.double", "4.231",
                          "truth", "test.truth", "False")
        provider.send("dump")
        self.assert_(provider.expect("Wrote ./context-provide.context"),
                     "context-provide.context couldn't been written by context-provide")
        info_client = CLTool("context-ls","-l","-d","test.*")

        self.assert_(info_client.expect(["^test.int\tINT\tcontextkit-dbus\tsession:com.nokia.test$",
                                         "^test.double\tDOUBLE\tcontextkit-dbus\tsession:com.nokia.test$",
                                         "^test.truth\tTRUTH\tcontextkit-dbus\tsession:com.nokia.test$",
                                         "^test.string\tSTRING\tcontextkit-dbus\tsession:com.nokia.test$",
                                         "^Documentation: A phony but very flexible property.$"]),
                     "Bad introspection result from context-ls")

        provider.close()

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
    sys.exit(runTests())
