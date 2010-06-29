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
import time
import unittest
from ContextKit.cltool import CLTool, wanted

class Asynchronous(unittest.TestCase):
    def testAsynchronicity(self):
        """
        Description
            This test verifies that the asynchronicity of the subscriber
            library

        Pre-conditions
            2 Providers started with context-provide tool.
            Each provider offers one property.
            1 provider is delayed (3s).

        Steps
            Subscribe to both properties
            Verify that return values are correct
            Measure the time elapsed between the reception of the 2 values.

        Post-conditions
            Kill providers

        References
            None
        """

        # start the client
        provider_slow = CLTool("context-provide", "--v2", "com.nokia.slow",
                               "int","test.slow","42")
        provider_slow.expect("Setting key") # wait for it
        provider_fast = CLTool("context-provide", "--v2", "com.nokia.fast",
                               "int","test.fast","44")
        provider_fast.expect("Setting key") # wait for it
        context_client = CLTool("context-listen")
        context_client.expect("Available commands") # wait for it

        provider_slow.comment("provider_slow sleep time started at" + str(time.time()))

        provider_slow.send("sleep 3")
        provider_slow.expect("Sleeping") # wait for it

        context_client.send("n test.slow ; n test.fast")

        # check the fast property
        self.assert_(context_client.expect(wanted("test.fast", "int", "44")), # timeout == 3 seconds
                     "Bad value for the fast property, wanted 44")
        fast_time = time.time()
        context_client.comment("Fast property arrived with good value at: " + str(fast_time))

        # check the slow property
        self.assert_(context_client.expect(wanted("test.slow", "int", "42")), # timeout == 10 seconds max, but 5 is enough usually
                     "Bad value for the slow property, wanted 42")
        slow_time = time.time()
        context_client.comment("Slow property arrived with good value at: " + str(slow_time))

        if slow_time - fast_time < 1.5:
            provider_slow.printio()
            context_client.printio()
            self.assert_(False,
                         "The arrival time of the fast and slow property is not far enough from each other")

        # context_client.printio()
        context_client.wait()
        provider_fast.wait()
        provider_slow.wait()

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(Asynchronous)

    result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    sys.exit(runTests())
