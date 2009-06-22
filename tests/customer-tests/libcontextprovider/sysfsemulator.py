import os
import os.path
import contexttest
import inotify
from twisted.internet import reactor
from servicetest import Event
from tempfile import mkdtemp

class SysfsEmulator:
    """
    Create a new emulated sysfs tree at the given location.
    Does not populat until files are added.
    The location should already exist
    """
    def __init__(self, queue):
        self.root_path = os.environ['CONTEXT_EXAMPLE_SYSFS_LOCATION']
        assert (os.access(self.root_path, os.R_OK | os.W_OK))
        self.i = inotify.INotify()
        self.queue = queue
        self.files = []

    def add_file(self, path, initial_value):
        (dirname,basename) = os.path.split(path)
        assert(basename != "")

        realdir  = self.root_path + dirname
        if not os.access(realdir, os.F_OK):
            os.makedirs(realdir)
        self.write(path, initial_value)
        self.files.append(path)

    def start(self):
        self.i.watch(unicode(self.root_path), auto_add = True, callbacks=(self._changed_cb,None), recursive=True)

    def stop(self):
        for name in self.files:
            os.remove(self.root_path + name)
        for root, dirs, files in os.walk(self.root_path, topdown=False):
            for name in dirs:
                os.rmdir(os.path.join(root, name))

    def read(self, path):
        f = open(self.root_path + path, 'r')
        contents = f.read()
        f.close()
        return contents

    def write(self, path, value):
        f = open(self.root_path + path, 'w')
        f.write(value)
        f.close()

    def _changed_cb(self, iwp, filename, mask, parameter=None):
        path = os.path.join(iwp.path, filename)
        assert(path.startswith(self.root_path))
        path = path[len(self.root_path):]
        if self.queue is not None:
            if mask & inotify.IN_MODIFY:
                self.queue.handle_event(Event('sysfs-modified', file=path, data=self.read(path)))

def test(q,bus):
    e = SysfsEmulator(q)
    e.add_file("/class/test/foo", "1")
    e.start()
    e.write("/class/test/foo", "0")
    q.expect('sysfs-modified', file="/class/test/foo", data="0")
    e.stop()

if __name__ == '__main__':
    from contexttest import exec_test
    exec_test(test)
