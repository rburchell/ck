#!/usr/bin/env python2.5
import os
import sys
import tempfile
import subprocess
import gobject
import dbus, dbus.service, dbus.mainloop.glib
import ContextProvider as CP

class _property(object):
    """Kind-of a template for property types."""
    def __init__(self, datatype, default):
        self.datatype = datatype
        self.default = default
        self.name = None
    def __call__(self, name, initial_value=None):
        """Clones self, setting its name and initial value."""
        if initial_value is None:
            initial_value = self.default
        prop = _property(self.datatype, initial_value)
        prop.name = name
        return prop

# Pre-defined types and their defaults.
INT = _property('INT', 0)
STRING = _property('STRING', '')
DOUBLE = _property('DOUBLE', 0.0)
TRUTH = _property('TRUTH', False)

def xmlfor(busname='ctx.flexiprovider', bus='session', *props):
    """Generates a provider .xml document for the given properties
    (which are instances of _property)."""
    xml = ['<?xml version="1.0"?>\n'
           '<provider bus="%s" service="%s">' % (bus, busname)]
    for p in props:
        xml.append('  <key name="%s">\n'
                   '    <type>%s</type>\n'
                   '    <doc>A phony but very flexible property.</doc>\n'
                   '  </key>' % (p.name, p.datatype))
    xml.append('</provider>\n')
    return '\n'.join(xml)

def update_context_providers(xml, dir='.'):
    """Dumps the xml into $dir/context-provide.context."""
    if "CONTEXT_PROVIDE_REGISTRY_FILE" in os.environ:
        outfilename = os.environ["CONTEXT_PROVIDE_REGISTRY_FILE"]
    else:
        outfilename = dir + '/context-provide.context'
    if outfilename.find('/') != -1:
        tmpdir = outfilename[:outfilename.rindex('/')]
    else:
        tmpdir = '.'
    tmpfd, tmpfn = tempfile.mkstemp('.contexttemp', 'context-provide-', tmpdir)
    os.write(tmpfd, xml)
    os.close(tmpfd)
    os.rename(tmpfn, outfilename)

class Flexiprovider(object):
    def stdin_ready(self, fd, cond):
        if cond & gobject.IO_ERR:
            self.loop.quit()
            return False
        # We assume that stdin is line-buffered (ie. readline() doesn't
        # block (too much)).  It's true if it's a tty.  If piping from
        # another program, do an entire line.
        # TODO maybe we should rather fix ourselves and accept arbitrary
        # amount of input (and accumulate lines ourselves).
        l = sys.stdin.readline()
        # If l is emtpy, we got EOF.
        if l == '':
            self.loop.quit()
            return False
        # Nevertheless, ignore emtpy lines.
        l = l.strip()
        if l == '':
            return True
        try:
            code = compile(l, '<input from stdin>', 'single')
            eval(code, {}, dict(INT=INT,
                                STRING=STRING,
                                TRUTH=TRUTH,
                                DOUBLE=DOUBLE,
                                set=self.set,
                                get=self.get,
                                reset=self.reset,
                                quit=self.loop.quit,
                                add=self.add_and_update,
                                info=self.info))
        except:
            # We are ignoring errors.
            import traceback
            traceback.print_exc()
        return True

    def __init__(self, properties, name='ctx.flexiprovider', bus='session'):
        self.busname = name
        self.bus = bus
        self.loop = gobject.MainLoop()
        self.props = {}
        self.values = {}
        # Hook into libcontextprovider.
        self.setters = dict(INT=CP.ContextProvider.set_integer,
                            STRING=CP.ContextProvider.set_string,
                            DOUBLE=CP.ContextProvider.set_double,
                            TRUTH=CP.ContextProvider.set_boolean)
        self.subscribed_cb = CP.ContextProvider.SUBSCRIPTION_CHANGED_CALLBACK(lambda x, y: None)
        retval = CP.ContextProvider.init(dict(session=0, system=1)[self.bus], self.busname)
        if retval == 0:
            raise Exception("libcontextprovider.init returned false")
        # Add properties and set the initial values.
        for p in properties:
            self.addproperty(p)
        self.update_providers()
        dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    def get(self, prop):
        return self.values.get(prop, None)

    def set(self, prop, val):
        if prop not in self.props:
            return
        self.values[prop] = val
        if val is None:
            CP.ContextProvider.set_null(prop)
        else:
            self.setters[self.props[prop].datatype](prop, val)

    def reset(self, prop):
        if prop not in self.props:
            return
        self.set(prop, self.props[prop].default)

    def addproperty(self, prop):
        self.props[prop.name] = prop
        self.values[prop.name] = prop.default
        CP.ContextProvider.install_key(prop.name, 0, self.subscribed_cb, None)
        self.reset(prop.name)

    def add_and_update(self, prop):
        self.addproperty(prop)
        self.update_providers()

    def update_providers(self):
        # Don't update the registry if we are being run as a commander.
        if self.busname == 'org.freedesktop.ContextKit.Commander':
            return
        update_context_providers(xmlfor(self.busname, self.bus, *self.props.values()))

    def info(self):
        """Dumps information about currently provided properties."""
        for p in self.props.values():
            print p.datatype.ljust(8), p.name.ljust(20), \
                self.values[p.name], '(', p.default, ')'

    def interactive(self):
        """Runs the provider interactively, accepting the following
        commands from the standard input (note that this is Python
        syntax, and it gets eval()ed):

          set(property, value):  sets the given property
          get(property):         prints the value of the property
          reset(property):       resets the property
          add(propertydesc):     adds a new property
          info():                prints properties and their values
          quit():                stops the whole thing

        The provider will stop also if stdin is closed.
        """
        # Reopen stdout to be line-buffered.
        sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
        gobject.io_add_watch(sys.stdin.fileno(),
                             gobject.IO_IN | gobject.IO_HUP | gobject.IO_ERR,
                             self.stdin_ready)
        self.loop.run()

    def run(self):
        self.loop.run()

__all__ = ('INT', 'STRING', 'DOUBLE', 'TRUTH', 'Flexiprovider')
