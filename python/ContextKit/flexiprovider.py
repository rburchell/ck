#!/usr/bin/python
import os
import sys
import tempfile
import subprocess
import glib, gobject
import dbus, dbus.service, dbus.mainloop.glib

def pkgconfig(*args):
    """Runs `pkg-config $args` and returns the Popen object, augmented
    with an `output' attribute containing stdout."""
    cmd = ['pkg-config'] + list(args)
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    process.output = process.communicate()[0].strip()
    return process

# The following kludge is needed if we want this to be usable without
# installing libcontextprovider.  If pkg-config reports it as
# uninstalled, we extend PYTHONPATH, then we try to import the module.
# If that fails, extend LD_LIBRARY_PATH and re-exec ourselves.

if pkgconfig('--exists', 'contextprovider-1.0').returncode != 0 or \
   pkgconfig('--exists', 'duivaluespace-1.0').returncode != 0:
    raise RuntimeError("You need to make pkg-config find "
                       "contextprovider-1.0 and duivaluespace-1.0 "
                       "somehow.\nTry setting $PKG_CONFIG_PATH.")

if pkgconfig('--uninstalled', 'contextprovider-1.0').returncode == 0:
    sys.path.append(pkgconfig('--variable=pythondir', 'contextprovider-1.0').output)
try:
    import ContextProvider as CP
except ImportError:
    raise
except:
    # Failed, probably because LD_LIBRARY_PATH is not right.  Set it and
    # re-exec ourselves.  To avoid an infinite loop, we try this only
    # when LD_LIBRARY_PATH doesn't yet contain what we want to add.
    libdir = pkgconfig('--variable=libdir', 'contextprovider-1.0').output
    ldpath = [d for d in os.environ.get('LD_LIBRARY_PATH', '').split(':') if d != '']
    if libdir in ldpath:
        raise
    ldpath += [libdir, libdir + '/.libs']
    env = dict(os.environ)
    env.update(LD_LIBRARY_PATH=':'.join(ldpath))
    os.execve(sys.argv[0], sys.argv, env)

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
        node, _, key = p.name.partition('.')
        xml.append('  <node name="%s">\n'
                   '    <key name="%s" type="%s">\n'
                   '       <doc>A phony but very flexible property.</doc>\n'
                   '    </key>\n'
                   '  </node>' % (node, key, p.datatype))
    xml.append('</provider>')
    return '\n'.join(xml)

# Location of the update-context-providers script, initialized lazily
# in the function below.
U_C_P = None
def update_context_providers(xml, dir='.'):
    """Runs the update-context-provider script in $dir after writing
    $xml to a temporary file."""
    global U_C_P
    if U_C_P is None:
        U_C_P = ''
        if pkgconfig('--uninstalled', 'duivaluespace-1.0').returncode == 0:
            U_C_P = '%s/registry/' % pkgconfig('--variable=pcfiledir',
                                              'duivaluespace-1.0').output
        U_C_P += 'update-context-providers'
    tmpf = tempfile.NamedTemporaryFile('w+b', -1, '.xml', 'flexi', dir)
    print >>tmpf, xml
    tmpf.flush()
    subprocess.call([U_C_P, dir])

class Flexiprovider(object):
    def stdin_ready(self, fd, cond):
        if cond & (glib.IO_ERR | glib.IO_HUP):
            self.loop.quit()
            return False
        # We assume that stdin is line-buffered (ie. readline() doesn't
        # block (too much)).  It's true if it's a tty.  If piping from
        # another program, do an entire line.
        # TODO maybe we should rather fix ourselves
        l = sys.stdin.readline().strip()
        if l == '':
            self.loop.quit()
            return False
        try:
            # TODO something more sophisticated?
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
        CP.ContextProvider.init(dict(session=0, system=1)[self.bus], self.busname)
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
        glib.io_add_watch(sys.stdin.fileno(),
                          glib.IO_IN | glib.IO_HUP | glib.IO_ERR,
                          self.stdin_ready)
        self.loop.run()

    def run(self):
        self.loop.run()

__all__ = ('INT', 'STRING', 'DOUBLE', 'TRUTH', 'Flexiprovider')
