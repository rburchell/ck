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
##   - starts up a provider with one property
##   - starts a client, verifies that the value arrives
##   - suspends the client (so it stops processing D-Bus messages)
##   - sets new value for the property twice in the provider
##   - resumes the client
##   - verifies that the client receives only one valueChanged signal

import re
import sys
import os
import unittest
from ContextKit.cltool import CLTool, wanted

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
        provider_fast.expect("Setting key")
        context_client = CLTool("context-listen", "test.fast")
        self.assert_(context_client.expect(wanted("test.fast", "int", "44")),
                     "Bad value for the fast property, wanted 44")

        context_client.suspend()
        provider_fast.send("test.fast = 34")
        provider_fast.expect("Setting key")
        provider_fast.send("test.fast = 54")
        provider_fast.expect("Setting key")
        context_client.resume()

        context_client.expect(wanted("test.fast", "int", "54"))

        # the two value changes can happen very close in time, so we
        # have to check that beyond the good value there are no other
        # value(s)
        if len(re.findall("test.fast =", context_client.last_output)) != 1:
            context_client.printio()
            self.assert_(False,
                         "expected a single valueChanged")

        # not even after waiting one second
        self.assertFalse(context_client.expect("test.fast =", wantdump = False, timeout=1),
                         "expected a single valueChanged")

        context_client.wait()
        provider_fast.wait()

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(RapidChanges)
    result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    sys.exit(runTests())
