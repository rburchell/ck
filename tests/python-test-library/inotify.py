# Licensed under the MIT license
# http://opensource.org/licenses/mit-license.php

# Copyright 2006,2007 Frank Scholz <coherence@beebits.net>
# Modified by Colin Laplace, added is_watched() function

import os
import os.path
import struct

try:
    import ctypes
except ImportError:
    ctypes = None


from twisted.internet import reactor
from twisted.internet.abstract import FileDescriptor
from twisted.internet import fdesc, protocol

from twisted.python.filepath import FilePath


# from /usr/src/linux/include/linux/inotify.h

IN_ACCESS =         0x00000001L     # File was accessed
IN_MODIFY =         0x00000002L     # File was modified
IN_ATTRIB =         0x00000004L     # Metadata changed
IN_CLOSE_WRITE =    0x00000008L     # Writtable file was closed
IN_CLOSE_NOWRITE =  0x00000010L     # Unwrittable file closed
IN_OPEN =           0x00000020L     # File was opened
IN_MOVED_FROM =     0x00000040L     # File was moved from X
IN_MOVED_TO =       0x00000080L     # File was moved to Y
IN_CREATE =         0x00000100L     # Subfile was created
IN_DELETE =         0x00000200L     # Subfile was delete
IN_DELETE_SELF =    0x00000400L     # Self was deleted
IN_MOVE_SELF =      0x00000800L     # Self was moved
IN_UNMOUNT =        0x00002000L     # Backing fs was unmounted
IN_Q_OVERFLOW =     0x00004000L     # Event queued overflowed
IN_IGNORED =        0x00008000L     # File was ignored

IN_ONLYDIR =         0x01000000      # only watch the path if it is a directory
IN_DONT_FOLLOW =     0x02000000      # don't follow a sym link
IN_MASK_ADD =        0x20000000      # add to the mask of an already existing watch
IN_ISDIR =           0x40000000      # event occurred against dir
IN_ONESHOT =         0x80000000      # only send event once

IN_CLOSE =      IN_CLOSE_WRITE | IN_CLOSE_NOWRITE   # closes
IN_MOVED =      IN_MOVED_FROM | IN_MOVED_TO         # moves
IN_CHANGED =    IN_MODIFY | IN_ATTRIB               # changes

IN_WATCH_MASK = IN_MODIFY | IN_ATTRIB | \
                IN_CREATE | IN_DELETE | \
                IN_DELETE_SELF | IN_MOVE_SELF | \
                IN_UNMOUNT | IN_MOVED_FROM | IN_MOVED_TO


_flag_to_human = {
    IN_ACCESS: 'access',
    IN_MODIFY: 'modify',
    IN_ATTRIB: 'attrib',
    IN_CLOSE_WRITE: 'close_write',
    IN_CLOSE_NOWRITE: 'close_nowrite',
    IN_OPEN: 'open',
    IN_MOVED_FROM: 'moved_from',
    IN_MOVED_TO: 'moved_to',
    IN_CREATE: 'create',
    IN_DELETE: 'delete',
    IN_DELETE_SELF: 'delete_self',
    IN_MOVE_SELF: 'move_self',
    IN_UNMOUNT: 'unmount',
    IN_Q_OVERFLOW: 'queue_overflow',
    IN_IGNORED: 'ignored',
    IN_ONLYDIR: 'only_dir',
    IN_DONT_FOLLOW: 'dont_follow',
    IN_MASK_ADD: 'mask_add',
    IN_ISDIR: 'is_dir',
    IN_ONESHOT: 'one_shot'}

# system call numbers are architecture-specific
# see /usr/include/linux/asm/unistd.h and look for inotify
_inotify_syscalls = { 'i386': (291,292,293),  # FIXME, there has to be a better way for this
                      'i486': (291,292,293),
                      'i586': (291,292,293),
                      'i686': (291,292,293),
                      'x86_64': (253,254,255), # gotten from FC-6 and F-7
                      'armv6l':(316,317,318),              # Nokia N800
                      'armv5tej1':(316,317,318),           # Nokia N770
                      'ppc': (275,276,277),                # PPC, like PS3
                      }


class IWatchPoint:

    def __init__(self, path, mask = IN_WATCH_MASK, auto_add = False, callbacks=[]):
        self.path = path
        self.mask = mask
        self.auto_add = auto_add
        self.callbacks = []
        if callbacks != None:
            if not isinstance(callbacks, list):
                callbacks = [callbacks]
            self.callbacks = callbacks

    def add_callback(self, callback, parameter=None):
        self.callbacks.append((callback,parameter))

    def remove_callback(self, callback):
        try:
            del self.callbacks[callback]
        except:
            pass

    def notify(self, filename, events):
        for callback in self.callbacks:
            if callback != None:
                callback[0](self, filename, events, callback[1])

class INotify(FileDescriptor, object):
    _instance_ = None  # Singleton

    def __new__(cls, *args, **kwargs):
        obj = getattr(cls,'_instance_',None)
        if obj is not None:
            return obj
        else:

            if ctypes == None:
                raise SystemError, "ctypes not detected on this system, INotify support disabled"

            obj = super(INotify, cls).__new__(cls, *args, **kwargs)
            try:
                obj.libc = ctypes.CDLL("libc.so.6")
            except:
                raise SystemError, "libc not found, INotify support disabled"

            try:
                obj.inotify_init = obj.libc.inotify_init
                #print "horray, we have a libc with inotify support"
                obj.inotify_add_watch = obj.libc_inotify_add_watch
                obj.inotify_rm_watch = obj.libc_inotify_rm_watch
            except:
                import platform
                machine = platform.machine()
                try:
                    obj._init_syscall_id = _inotify_syscalls[machine][0]
                    obj._add_watch_syscall_id = _inotify_syscalls[machine][1]
                    obj._rm_watch_syscall_id = _inotify_syscalls[machine][2]
                except:
                    raise SystemError, "unknown system '%s', INotify support disabled" % machine

            FileDescriptor.__init__(obj)

            obj._fd = obj.inotify_init()
            if obj._fd < 0:
                raise SystemError, "INotify support not detected on this system."

            fdesc.setNonBlocking(obj._fd) # FIXME do we need this?

            reactor.addReader(obj)

            obj._buffer = ''
            obj._watchpoints = {}
            cls._instance_ = obj
            return obj

    def release(self):
        reactor.removeReader(self)
        if hasattr(self, '_fd') and self._fd >= 0:
            try:
                os.close(self._fd)
            except OSError:
                pass

        if hasattr(INotify, '_instance_'):
            del INotify._instance_

    __del__ = release

    def inotify_init(self):
        return self.libc.syscall(self._init_syscall_id)

    def inotify_add_watch(self, path, mask):
        if type(path) is unicode:
            path = path.encode('utf-8')
            self.libc.syscall.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
        else:
            self.libc.syscall.argtypes = None
        return self.libc.syscall(self._add_watch_syscall_id, self._fd, path, mask)

    def inotify_rm_watch(self, wd):
        return self.libc.syscall(self._rm_watch_syscall_id, self._fd, wd)

    def libc_inotify_add_watch(self, path, mask):
        if type(path) is unicode:
            path = path.encode('utf-8')
            self.libc.inotify_add_watch.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
        else:
            self.libc.inotify_add_watch.argtypes = None
        return self.libc.inotify_add_watch(self._fd, path, mask)

    def libc_inotify_rm_watch(self, wd):
        return self.libc.inotify_rm_watch(self._fd, wd)

    def fileno(self):
        return self._fd

    def flag_to_human(self, mask):
        s = []
        for (k, v) in _flag_to_human.iteritems():
            if k & mask:
                s.append(v)
        return s

    def notify(self, iwp, filename, mask, parameter=None):
        print "event %s on %s" % (
            ', '.join(self.flag_to_human(mask)), os.path.join(iwp.path, filename))

    def doRead(self):
        self._buffer += os.read(self._fd, 1024)

        while True:
            if len(self._buffer) < 16:
                break

            wd, mask, cookie, size = struct.unpack("=LLLL", self._buffer[0:16])
            if size:
                name = self._buffer[16:16+size].rstrip('\0')
            else:
                name = None

            self._buffer = self._buffer[16+size:]

            try:
                iwp = self._watchpoints[wd]
            except:
                continue # can this happen?

            path = iwp.path
            if name:
                name = unicode(name, 'utf-8')
                path = os.path.join(path, name)

                iwp.notify( name, mask)

            if( iwp.auto_add and mask & IN_ISDIR and mask & IN_CREATE):
                self.watch(path, mask = iwp.mask, auto_add = True, callbacks=iwp.callbacks)

            if mask & IN_DELETE_SELF:
                # watch point got deleted, remove its data.
                del self._watchpoints[wd]


    def watch(self, path, mask = IN_WATCH_MASK, auto_add = None, callbacks=[], recursive=False):
        if recursive:
            for root, dirs, files in os.walk(path):
                self.watch(root, mask, auto_add, callbacks, False)
        else:
            if isinstance(path, FilePath):
                path = path.path
            path = os.path.realpath(path)
            for wd, iwp in self._watchpoints.items():
                if iwp.path == path:
                    return wd

            mask = mask | IN_DELETE_SELF

            #print "add watch for", path, ', '.join(self.flag_to_human(mask))
            wd = self.inotify_add_watch(path, mask)
            if wd < 0:
                raise IOError, "Failed to add watch on '%s' - (%r)" % (path.encode('ascii', 'ignore'),wd)

            iwp = IWatchPoint(path, mask, auto_add, callbacks)
            self._watchpoints[wd] = iwp

    def ignore(self, path):
        path = os.path.realpath(path)
        found_wd = None
        for wd, iwp in self._watchpoints.items():
            if iwp.path == path:
                self.inotify_rm_watch(wd)
                del self._watchpoints[wd]
                break

    def is_watched(self, path):
        for wd, iwp in self._watchpoints.items():
            if iwp.path == path:
                return True
        return False

if __name__ == '__main__':

    i = INotify()
    print i
    i.watch(unicode('/tmp'), auto_add = True, callbacks=(i.notify,None), recursive=True)

    i2 = INotify()
    print i2
    i2.watch('/', auto_add = True, callbacks=(i2.notify,None), recursive=False)
    reactor.run()
