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

import sys
import os
import time
import unittest
from ContextKit.cltool import CLTool

property_name = ""

class HasProvider(unittest.TestCase):
    def test_has_provider(self):
        global property_name
        # start listening to the property
        context_client = CLTool("context-listen", property_name)

        context_client.send("providers " + property_name)

        # assert that there is at least one provider; plugin and
        # constructionstring are separated with @ in the provider
        # desription
        self.assert_(context_client.expect("@"))

def runTests():
    suite = unittest.TestLoader().loadTestsFromTestCase(HasProvider)

    result = unittest.TextTestRunner(verbosity=2).run(suite)
    return len(result.errors + result.failures)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print >> sys.stderr, "Usage:", sys.argv[0], "PROPERTY_NAME"
        exit(1)

    property_name = sys.argv[1]

    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
    sys.exit(runTests()) # cannot use unittests.main because of the parameters
