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
from ContextKit.cltool import CLTool

class MultiProvider(unittest.TestCase):
    def testMultipleProviders(self):
        """
        Description
	    This test verifies correct client behavior in the presence of
	    multiple providers.

        Steps
            1. starts up a client
            2. starts two providers (X and Y) providing the same P property
            3. X sets P to V1 and verifies that the client got it
	    4. Y sets P to V2 and likewise verifies in the client
	    5. Y sets P to NULL, the client verifies that P goes back to V1
	    6. Y sets P to V3, the client verifies P == V3
	    7. Y is removed from the registry, client verifies that P == V1
	    8. X is removed from the registry, client verifies that P == NULL
        """
        client = CLTool("context-listen", "test.prop")
	client.expect(CLTool.STDERR, "Available commands", 3)

        provider_x = CLTool("context-provide", "--v2", "test.X",
			    "int", "test.prop", "44")
	provider_x.send("dump x.context")
        provider_x.expect(CLTool.STDOUT, "Wrote", 10)

        provider_y = CLTool("context-provide", "--v2", "test.Y",
			    "int", "test.prop", "22")
	provider_y.send("dump y.context")
        provider_y.expect(CLTool.STDOUT, "Wrote", 10)

	provider_x.send("test.prop = 55");
        provider_x.expect(CLTool.STDOUT, "Setting key", 10)
        self.assert_(client.expect(CLTool.STDOUT,
				   CLTool.wanted("test.prop", "int", "55"),
				   3))

	provider_y.send("test.prop = 77");
        provider_y.expect(CLTool.STDOUT, "Setting key", 10)
        self.assert_(client.expect(CLTool.STDOUT,
				   CLTool.wanted("test.prop", "int", "77"),
				   3))

	provider_y.send("unset test.prop");
        provider_y.expect(CLTool.STDOUT, "Setting key", 10)
        self.assert_(client.expect(CLTool.STDOUT,
				   CLTool.wanted("test.prop", "int", "55"),
				   3))

	provider_y.send("test.prop = 99");
        provider_y.expect(CLTool.STDOUT, "Setting key", 10)
        self.assert_(client.expect(CLTool.STDOUT,
				   CLTool.wanted("test.prop", "int", "99"),
				   3))

	provider_y.close()
	os.unlink("y.context")
        self.assert_(client.expect(CLTool.STDOUT,
				   CLTool.wanted("test.prop", "int", "55"),
				   3))

	provider_x.close()
	os.unlink("x.context")
        self.assert_(client.expect(CLTool.STDOUT,
				   CLTool.wantedUnknown("test.prop"),
				   3))
        client.close()

def runTests():
    suiteInstallation = unittest.TestLoader().loadTestsFromTestCase(MultiProvider)
    result = unittest.TextTestRunner(verbosity=2).run(suiteInstallation)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    sys.exit(runTests())
