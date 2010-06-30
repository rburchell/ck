#!/usr/bin/env python2.5
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
import unittest
import os
from ContextKit.cltool import CLTool

class Types(unittest.TestCase):
        def testTypeChangeSameValue(self):
                """
                Description
                This test verifies that valueChanged signals are
                emitted even if the new value is the same as old
                value, just the type is different.

                Steps
                1. starts a provider as commander with test.prop -> int:42
                2. starts a client, subscribes to test.prop, client
                   sees the value
                3. provider changes the value to double:42
                4  client sees the new value
                """
                provider = CLTool("context-provide")
                provider.send("add int test.prop 42")
                provider.expect("Added key")

                client = CLTool("client")

                client.send("assign session org.freedesktop.ContextKit.Commander commander")
                client.expect("Assigned commander")

                client.send("subscribe commander test.prop")
                self.assert_(client.expect("Subscribe returned: qulonglong:42"))

                provider.send("settype test.prop double")
                provider.send("test.prop = 42.0")
                provider.expect("Setting key")

                client.send("waitforchanged 5000")
                self.assert_(client.expect("ValueChanged: org.freedesktop.ContextKit.Commander /org/maemo/contextkit/test/prop double:42"))

                client.close()
                provider.close()

if __name__ == "__main__":
        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
        unittest.main()
