#!/usr/bin/python
"""rl.py -- a simple readline wrapper

Usage: ./rl.py [some other program and args]

Wraps the plain stdin-based input mechanism of the given program into a
convenient readline-based one.
"""

import os
import sys
import signal
import readline

if len(sys.argv) < 2:
    sys.exit(__doc__)
rfd, wfd = os.pipe()
pid = os.fork()
if pid == 0:
    os.close(wfd)
    os.dup2(rfd, 0)
    sys.argv.pop(0)
    os.execv(sys.argv[0], sys.argv)
else:
    def childied(n, f):
        p, ec = os.waitpid(pid, 0)
        sys.exit(ec)
    signal.signal(signal.SIGCHLD, childied)
    os.close(rfd)
    try:
        while True:
            l = raw_input()
            os.write(wfd, l + '\n')
    except (KeyboardInterrupt, SystemExit): raise
    except: pass
