#!/usr/bin/env python
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

from __future__ import with_statement

import os
import re
import sys
import time
import math
import signal
from threading import Thread, Lock
from subprocess import Popen, PIPE, STDOUT

class Reader(Thread):
    def __init__(self, cltool):
        Thread.__init__(self)
        self.cltool = cltool
        self.running = True

    def stop(self):
        self.running = False

    def run(self):
        while self.running:
            l = self.cltool.process.stdout.readline()
            if l:
                event = (time.time(), CLTool.STDOUT, l)
            else:
                event = (time.time(), CLTool.COMMENT, "EOF ON STDOUT")
                self.stop()
            with self.cltool.iolock:
                self.cltool.io.append(event)
        self.cltool.wait()

class CLTool:

    COMMENT = -1
    STDIN = 0
    STDOUT = 1

    def __init__(self, *cline):
        self.process = Popen(cline, stdin=PIPE, stdout=PIPE, stderr=STDOUT,
                             preexec_fn=self._preexec)
        self.io = []
        self.iolock = Lock()
        self.reader = Reader(self)
        self.last_expect = 0
        self.reader.setDaemon(True)
        self.reader.start()

    def __del__(self):
        self.reader.stop()
        self.reader.join()

    def _preexec(self):
        """Enable core dumps."""
        import resource
        resource.setrlimit(resource.RLIMIT_CORE, (-1, -1))

    def send(self, string):
        """Writes STRING to the standard input of the child."""
        with self.iolock:
            self.io.append((time.time(), CLTool.STDIN, string))
        print >>self.process.stdin, string
        self.process.stdin.flush()

    def _return_event(self, wantdump):
        with self.iolock:
            self.last_expect = len(self.io)
        if wantdump:
            self.printio()

    def _last_output(self):
        """Compute the output read since the last match."""
        r = []
        with self.iolock:
            for pos in xrange(self.last_expect, len(self.io)):
                ts, fno, l = self.io[pos]
                if fno == CLTool.STDOUT:
                    r.append(l)
        return ''.join(r)

    # exp: is either a single or a list of regexes and all of them has to match
    # timeout: the match has to be occur in this many seconds
    # wantdump: in case of error should we print a dump or not
    def expect(self, exp, timeout=5, wantdump=True):
        """Expect one more more regexps on the output.

        EXP is either a regular expression or a list of them
        `timeout'  -- specifies how many seconds to wait for the match [5]
        `wantdump' -- causes the io to be dumped if the expectation fails [True]

        """
        if not isinstance(exp, (list, tuple)):
            exp = [exp]
        rexp = [re.compile(s, re.MULTILINE) for s in exp]
        abs_timeout = time.time() + timeout
        while True:
            # check if we are matching
            # enumerate all of the patterns and remove the matching ones
            i = 0
            match_this = self._last_output()
            while i < len(rexp):
                if re.search(rexp[i], match_this):
                    del rexp[i]
                    del exp[i]
                    i -= 1
                i += 1
            if not rexp:
                self.comment('EXPECT OK')
                self._return_event(False)
                return True
            now = time.time()
            # timed out
            if now > abs_timeout:
                self.comment('TIMEOUT')
                self._return_event(wantdump)
                return False
            # stream is closed
            if not self.reader.running:
                self._return_event(wantdump)
                return False
            time.sleep(0.1)

    def comment(self, st):
        """Append a comment to the io log."""
        with self.iolock:
            self.io.append((time.time(), CLTool.COMMENT, st))

    def suspend(self):
        """Suspend the child with a SIGSTOP signal."""
        os.kill(self.process.pid, signal.SIGSTOP)

    def resume(self):
        """Resume the child with SIGCONT."""
        os.kill(self.process.pid, signal.SIGCONT)

    def close(self):
        """Close the standard input of the child."""
        self.comment("EOF ON STDIN")
        self.process.stdin.close()

    def wait(self):
        """Waits for the child (using wait(2))."""
        rc = self.process.wait()
        if rc < 0:
            self.comment("TERMINATED WITH SIGNAL %d" % -rc)
            self.printio()
        elif rc > 0:
            self.comment("EXIT CODE: %d" % rc)
        return rc

    def printio(self):
        """Dump the io log to the standard output."""
        print
        print '-' * 72
        with self.iolock:
            for ts, fno, line in self.io:
                line = line.rstrip("\r\n")
                t = "%s.%03d" % (time.strftime("%H:%M:%S", time.localtime(ts)),
                                 math.modf(ts)[0] * 1000)
                if fno == CLTool.COMMENT: info = "###";
                if fno == CLTool.STDIN:   info = "<--";
                if fno == CLTool.STDOUT:  info = "-->";
                print "%s %s %s" % (t, info, line)
        print '-' * 72
        sys.stdout.flush()

def wanted(name, type, value):
    """Construct an expect() regexp expecting a context property with VALUE."""
    return "^%s = %s:%s$" % (name, type, value)

def wantedUnknown(name):
    """Construct an expect() regexp expecting an unset context property."""
    return "^%s is Unknown$" % (name)

__all__ = ['wanted', 'wantedUnknown', 'CLTool']
