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
## Requires python2.5-gobject and python2.5-dbus
##

import sys
import unittest
import os
from ContextKit.cltool import CLTool

class MultiProvider(unittest.TestCase):
        def tearDown(self):
                #os.unlink('./context-provide.context')
                #os.unlink('./context-provide2.context')
                pass
        
        def testPingPong(self):
                """

                """
                provider = CLTool("context-provide", "--v2", "--system", "com.nokia.test",
                                  "int","test.int","1",
                                  "string","test.string","foobar",
                                  "double","test.double","2.5",
                                  "truth","test.truth","True")
                provider.send("dump")
                provider2 = CLTool("context-provide", "--v2", "--session", "com.nokia.test2",
                                  "int","test.int","1",
                                  "string","test.string","foobar",
                                  "double","test.double","2.5",
                                  "truth","test.truth","True")
                provider2.send("dump context-provide2.context")


if __name__ == "__main__":
        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
        unittest.main()
