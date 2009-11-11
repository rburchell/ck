#!/usr/bin/env python
##
## This file is part of ContextKit.
##
## Copyright (C) 2009 Nokia. All rights reserved.
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

class Subscription(unittest.TestCase):
        def tearDown(self):
                os.remove('time.context')

        def testChangingPlugin(self):
                os.environ["CONTEXT_PROVIDERS"] = "."
                if os.path.isdir("../testplugins"):
                        # for your local machine
                        os.environ["CONTEXT_SUBSCRIBER_PLUGINS"] = "../testplugins"
                else:
                        # if libcontextsubscriber-tests is installed
                        os.environ["CONTEXT_SUBSCRIBER_PLUGINS"] = "/usr/lib/contextkit/subscriber-test-plugins"

                if not os.path.exists(os.environ["CONTEXT_SUBSCRIBER_PLUGINS"] + "/" + "contextsubscribertime1.so") \
                            or not os.path.exists(os.environ["CONTEXT_SUBSCRIBER_PLUGINS"] + "/" + "contextsubscribertime2.so"):
                        self.assert_(False, "Couldn't find the test time plugins")

                self.context_client = CLTool("context-listen", "Test.Time")
                self.context_client.expect("Available commands") # wait for it

                # Copy the declaration file, declaring libcontextsubscribertime1 plugin.
                os.system('cp time1.context.temp time.context.temp')
                os.system('mv time.context.temp time.context')
                #print "now reading"

                # Expect value coming from plugin libcontextsubscribertime1
                self.assert_(self.context_client.expect("Test.Time = QString:\"Time1:"))

                # Modify the registry so that the key is now provided by libcontextsubscribertime2
                os.system('cp time2.context.temp time.context.temp')
                os.system('mv time.context.temp time.context')

                # Expect value coming from plugin libcontextsubscribertime2
                self.assert_(self.context_client.expect("Test.Time = QString:\"Time2:"))

if __name__ == "__main__":
        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
        unittest.main()
