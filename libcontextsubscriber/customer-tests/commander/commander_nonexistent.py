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
##       - starts up a client and a provider
##       - starts a commander with a different value and type for a provider
##         provided property and with a new string non-provided property
##       - checks that the client gets an error for the provider provided property
##       - checks that the client gets the new string non-provided property
##       - changes the non-provided property's type to int and checks it
##         (so this proves that type checks are always off for commander properties)

import sys
import os
import unittest
from ContextKit.cltool import CLTool

class CommanderNonExistent(unittest.TestCase):
        def tearDown(self):
                os.unlink('context-provide.context')

        def testCommanderFunctionality(self):
                provider = CLTool("context-provide", "--v2", "com.nokia.test", "int", "test.int", "42")
                provider.send("dump")
                self.assert_(provider.expect(CLTool.STDOUT, "Wrote", 10)) # wait for it
                listen = CLTool("context-listen", "test.int", "test.string")
                commander = CLTool("context-provide", "--v2")
                commander.send("add string test.int foobar")
                commander.send("add string test.string barfoo")
                commander.send("start")
                commander.expect(CLTool.STDOUT, "Added", 10) # wait for it
                commander.expect(CLTool.STDOUT, "Added", 10) # wait for it

                self.assert_(listen.expect(CLTool.STDERR,
                                           'Provider error, bad type for  "test.int" wanted: "INT" got: QString',
                                           1),
                             "Type check didn't work")

                # check the non-existent property
                self.assert_(listen.expect(CLTool.STDOUT,
                                           CLTool.wanted("test.string", "QString", "barfoo"),
                                           1),
                     "Non-existent property couldn't be commanded")

                # change the type of the non-existent property
                commander.send("add int test.string 42")
                self.assert_(listen.expect(CLTool.STDOUT,
                                           CLTool.wanted("test.string", "int", "42"),
                                           1),
                             "Non-existent property's type couldn't be overwritten")

def runTests():
        suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(CommanderNonExistent)
        result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
        return len(result.errors + result.failures)

if __name__ == "__main__":
        sys.exit(runTests())
