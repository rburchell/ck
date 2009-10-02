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
import unittest
from ContextKit.cltool import CLTool

class CommanderDisabled(unittest.TestCase):
    def tearDown(self):
        os.unlink('context-provide.context')

    def testCommanderFunctionality(self):
        provider = CLTool("context-provide", "--v2", "contextkit.test", "int", "test.int", "42")
        provider.send("dump")
        provider.expect(CLTool.STDOUT, "Wrote", 10) # wait for it
        commander = CLTool("context-provide", "--v2")
        commander.send("add int test.int 4242")
        commander.send("start")
        commander.expect(CLTool.STDOUT, "Added", 10) # wait for it
        os.environ["CONTEXT_CLI_IGNORE_COMMANDER"] = ""
        listen = CLTool("context-listen", "test.int")
        self.assert_(listen.expect(CLTool.STDOUT,
                                   CLTool.wanted("test.int", "int", "42"),
                                   1),
                     "Provider provided value is wrong")

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(CommanderDisabled)
    result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    sys.exit(runTests())
