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
import string
from subprocess import Popen, PIPE
import time
import signal
from cltool import CLTool

def timeoutHandler(signum, frame):
	raise Exception('tests have been running for too long')

def stdoutRead (object,lines):
	list = []
	for i in range(lines):
		list.append(object.stdout.readline().rstrip())
	return list

class Subscription(unittest.TestCase):

	def setUp(self):
		os.environ["CONTEXT_PROVIDERS"] = "."
		# We need 2 plugins which are in separate directories.
		os.environ["CONTEXT_SUBSCRIBER_PLUGINS"] = "."
		os.system('cp ../testplugins/libcontextsubscribertime1.so.0.0.0 ./libcontextsubscribertime1.so')
		os.system('cp ../testplugins/libcontextsubscribertime2.so.0.0.0 ./libcontextsubscribertime2.so')

		self.context_client = CLTool("context-listen", "Test.Time")

	def tearDown(self):
		os.remove('time.context')
		os.system('rm libcontextsubscribertime*.so*')

	def testChangingPlugin(self):

		# Copy the declaration file, declaring libcontextsubscribertime1 plugin.
		os.system('cp time1.context.temp time.context.temp')
		os.system('mv time.context.temp time.context')
		#print "now reading"

		# Expect value coming from plugin libcontextsubscribertime1
		self.assert_(self.context_client.expect(CLTool.STDOUT, "Test.Time = QString:Time1:", 3))

		# Modify the registry so that the key is now provided by libcontextsubscribertime2
		os.system('cp time2.context.temp time.context.temp')
		os.system('mv time.context.temp time.context')

		# Expect value coming from plugin libcontextsubscribertime2
		self.assert_(self.context_client.expect(CLTool.STDOUT, "Test.Time = QString:Time2:", 3))

if __name__ == "__main__":
	sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
	signal.signal(signal.SIGALRM, timeoutHandler)
	signal.alarm(30)
	unittest.main()