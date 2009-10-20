#!/usr/bin/env python
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
##   - starts up a provider with one property
##   - starts a client, verifies that the value arrives
##   - suspends the client (so it stops processing D-Bus messages)
##   - sets new value for the property twice in the provider
##   - resumes the client
##   - verifies that the client receives only one valueChanged signal

import sys
import os
import unittest
from ContextKit.cltool import CLTool

class RapidChanges(unittest.TestCase):
    def testRapidChanges(self):
        """
        Description
            This test verifies that the subscriber library doesn't emit
            unnecessary valueChanged signals (in spite of receiving multiple
            D-Bus messages in the same main loop iteration).

        Steps
            1. starts up a provider with one property
            2. starts a client, verifies that the value arrives
            3. suspends the client (so it stops processing D-Bus messages)
            4. sets new value for the property twice in the provider
            5. resumes the client
            6. verifies that the client receives only one valueChanged signal
        """

        provider_fast = CLTool("context-provide", "--v2", "com.nokia.fast",
                               "int","test.fast","44")
        provider_fast.expect(CLTool.STDOUT, "Setting key", 10)
        context_client = CLTool("context-listen", "test.fast")
        self.assert_(context_client.expect(CLTool.STDOUT,
                                           CLTool.wanted("test.fast", "int", "44"),
                                           3), # timeout == 3 seconds
                     "Bad value for the fast property, wanted 44")

        context_client.suspend()
        provider_fast.send("test.fast = 34")
        provider_fast.expect(CLTool.STDOUT, "Setting key", 10)
        provider_fast.send("test.fast = 54")
        provider_fast.expect(CLTool.STDOUT, "Setting key", 10)
        context_client.resume()
        # /.^/ is a regexp that doesn't match anything
        context_client.expect(CLTool.STDOUT, ".^", 3, wantdump=False)
        self.assertEqual(context_client.last_output, "test.fast = int:54\n",
                         "expected a single valueChanged")

        provider_fast.close()
        context_client.close()

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(RapidChanges)
    result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    sys.exit(runTests())
