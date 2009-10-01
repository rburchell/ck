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

import re
import time
from pprint import pprint
from subprocess import Popen, PIPE
from fcntl import fcntl, F_GETFL, F_SETFL
from os import O_NONBLOCK, system

class CLTool:
    STDOUT = 1
    STDERR = 2
    def __init__(self, *cline):
        self.__process = Popen(cline, stdin=PIPE, stdout=PIPE, stderr=PIPE)
        self.__io = []

    def send(self, string):
        self.__io.append((0, string))
        print >>self.__process.stdin, string
        self.__process.stdin.flush()

    def expect(self, fileno, exp_str, timeout, wantdump = True):
        return self.expectAll(fileno, [exp_str], timeout, wantdump)

    # fileno: can be either CLTool.STDOUT or CLTool.STDERR
    # _exp_l: is a list of regexes and all of them has to match
    # timeout: the match has to be occur in this many seconds
    # wantdump: in case of error should we print a dump or not
    def expectAll(self, fileno, _exp_l, timeout, wantdump = True):
        exp_l = list(_exp_l)
        stream = 0
        if fileno == self.STDOUT: stream = self.__process.stdout
        if fileno == self.STDERR: stream = self.__process.stderr
        if stream == 0: return False

        # set the stream to nonblocking
        fd = stream.fileno()
        flags = fcntl(fd, F_GETFL)
        fcntl(fd, F_SETFL, flags | O_NONBLOCK)

        cur_str = ""
        start_time = time.time()
        while True:
            try:
                line = stream.readline()
                if not line: # eof
                    self.__io.append((fileno, "--------- STREAM UNEXPECTEDLY CLOSED -----------"))
                    if wantdump:
                        self.printio()
                        print "Expected:"
                        pprint(exp_l)
                    self.__io.append((fileno, "--------- EXPECT ERR -----------"))
                    return False
                line = line.rstrip()
                cur_str += line + "\n"
                self.__io.append((fileno, line))
            except:
                time.sleep(0.1) # no data were available
            # enumerate all of the patterns and remove the matching ones
            i = 0
            while i < len(exp_l):
                # this is a hack, so you can have expressions like "\nwhole line\n"
                # and they will match even for the first line
                if re.search(exp_l[i], "\n" + cur_str):
                    del(exp_l[i])
                    i -= 1
                i += 1
            if exp_l:
                if time.time() - start_time > timeout: # timeout
                    if wantdump:
                        self.printio()
                        print "Expected:"
                        pprint(exp_l)
                    self.__io.append((fileno, "--------- EXPECT ERR -----------"))
                    return False
            else:
                # if no more patterns to match, then we are done
                self.__io.append((fileno, "--------- EXPECT  OK -----------"))
                return True

    def comment(self, st):
        self.__io.append((-1, st))

    def kill(self):
        system('../common/rec-kill.sh %d' % self.__process.pid)
        # can be changed to this, when python 2.6 is everywhere on maemo:
        #self.__process.kill()

    def wait(self):
            return self.__process.wait()

    def printio(self):
        print
        print '----------------------------------------------------'
        for line in self.__io:
            if line[0] == 0:
                print "[IN] <<<", line[1]
            if line[0] == 1:
                print "[OU] >>>", line[1]
            if line[0] == 2:
                print "[ER] >>>", line[1]
            if line[0] == -1:
                print "[CM] ===", line[1]
        print '----------------------------------------------------'

    def wanted(name, type, value):
        return "\n%s = %s:%s\n" % (name, type, value)
    wanted = staticmethod(wanted)

    def wantedUnknown(name):
        return "\n%s is Unknown\n" % (name)
    wantedUnknown = staticmethod(wantedUnknown)
