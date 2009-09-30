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
from ContextKit.cltool import CLTool

def timeoutHandler(signum, frame):
    raise Exception('tests has been running for too long')

class Callable:
    def __init__(self, anycallable):
        self.__call__ = anycallable

class CommanderAppearing(unittest.TestCase):
    def tearDown(self):
        os.unlink('context-provide.context')

    def testCommanderFunctionality(self):
        provider = CLTool("new-context-provide", "com.nokia.test", "int", "test.int", "42")
        provider.send("dump")

        listen = CLTool("context-listen", "test.int")

        self.assert_(listen.expect(CLTool.STDOUT,
                                           CLTool.wanted("test.int", "int", "42"),
                                           1),
                     "Bad value initially from the real provider, wanted 42")

        commander =  CLTool("new-context-provide")
        commander.send("add int test.int 4242")
        commander.send("start")

        self.assert_(listen.expect(CLTool.STDOUT,
                                           CLTool.wanted("test.int", "int", "4242"),
                                           1),
                     "Value after commander has been started is wrong, wanted 4242")

        commander.send("unset test.int")
        listen.comment("commander commanded test.int to unknown")
        self.assert_(listen.expect(CLTool.STDOUT,
                                           CLTool.wantedUnknown("test.int"),
                                           1),
                     "Value after commander has changed it to unknown is wrong")

        commander.send("test.int = 1235")
        self.assert_(listen.expect(CLTool.STDOUT,
                                           CLTool.wanted("test.int", "int", "1235"),
                                           1),
                     "Value after commander has changed it is wrong, wanted 1235")

        commander.kill()
        listen.comment("Commander killed")
        self.assert_(listen.expect(CLTool.STDOUT,
                                           CLTool.wanted("test.int", "int", "42"),
                                           1),
                     "Value after killing the commander is wrong, wanted 42")

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(CommanderAppearing)
    result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    signal.signal(signal.SIGALRM, timeoutHandler)
    signal.alarm(10)
    sys.exit(runTests())
