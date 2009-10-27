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

import sys
import os
import unittest
import time
from ContextKit.cltool import CLTool

class MultiProvider(unittest.TestCase):
    def tearDown(self):
	try:
	        os.unlink("x.context")
	except:
		pass
	try:
	        os.unlink("y.context")
	except:
		pass

    def testMultipleProviders2(self):
        """
        Description
            This test verifies correct client behavior in the presence
            of multiple providers.  In this test we always start the
            client after the providers have already been started and
            values have been set.

        Steps
            1. starts two providers (X and Y) providing the same P property
            2. X sets P to V1
            3. Y sets P to V2
            4. starts a client and check that value for P is V2
            5. starts a client with different provider order and
               checks that value for P is still V2
        """
	# please reenable this test when new protocol on provider side is merged!
	return

        provider_x = CLTool("context-provide", "--v2", "test.x",
                            "int", "test.prop", "44")
        provider_x.send("dump x.context")
        provider_x.expect(CLTool.STDOUT, "Wrote", 10)

        provider_y = CLTool("context-provide", "--v2", "test.y",
                            "int", "test.prop", "22")
        provider_y.send("dump y.context")
        provider_y.expect(CLTool.STDOUT, "Wrote", 10)

        client = CLTool("context-listen")
        client.expect(CLTool.STDERR, "Available commands", 3)
        provider_x.send("sleep 2")
        provider_x.expect(CLTool.STDOUT, "Sleeping", 10)
        client.send("n test.prop")

        time.sleep(4)

        client.send("value test.prop")
        self.assert_(client.expect(CLTool.STDOUT,
                                   "\nvalue: int:22\n",
                                   3))
        client.close()
        provider_x.close()
        provider_y.close()

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(MultiProvider)
    result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    sys.exit(runTests())
