from ctypes import *
from CTypesHelpers import *


class STRING_SET(c_void_p):
    pass

class CHANGE_SET(c_void_p):
    pass

class ContextProvider:
    def __init__(self):
        self.define_types()
        self.define_functions()

    def define_types(self):
        self.dll = CDLL("libcontextprovider.so")
        self.STRING_ARRAY = POINTER(c_char_p)
        self.GET_CALLBACK = POINTER(CFUNCTYPE(STRING_SET, CHANGE_SET, c_void_p))
        self.SUBSCRIBE_CALLBACK = POINTER(CFUNCTYPE(STRING_SET, c_void_p))

    def define_functions(self):
        self.init = cfunc('context_provider_init', self.dll, None,
                            ('provided_keys', ListPOINTER (c_char_p), 1),
                            ('useSessionBus', c_int, 1),
                            ('get_cb', self.GET_CALLBACK, 1),
                            ('get_cb_target', c_void_p, 1),
                            ('first_cb', self.SUBSCRIBE_CALLBACK, 1),
                            ('first_cb_target', c_void_p, 1),
                            ('last_cb', self.SUBSCRIBE_CALLBACK, 1),
                            ('last_cb_target', c_void_p, 1))
        self.no_of_subscribers = cfunc('context_provider_no_of_subscribers', self.dll, c_int,
                                    ('key', c_char_p, 1))

        self.change_set_create = cfunc('context_provider_change_set_create', self.dll, CHANGE_SET)
        self.change_set_commit = cfunc('context_provider_change_set_commit', self.dll, None,
                                    ('change_set', CHANGE_SET, 1))
        self.change_set_cancel = cfunc('context_provider_change_set_cancel', self.dll, None,
                                    ('change_set', CHANGE_SET, 1))

        self.change_set_add_int = cfunc('context_provider_change_set_add_int', self.dll, c_int,
                                    ('change_set', CHANGE_SET, 1),
                                    ('key', c_char_p, 1),
                                    ('val', c_int, 1))
        self.change_set_add_double = cfunc('context_provider_change_set_add_double', self.dll, c_int,
                                    ('change_set', CHANGE_SET, 1),
                                    ('key', c_char_p, 1),
                                    ('val', c_double, 1))

        self.change_set_add_undetermined_key = cfunc('context_provider_change_set_add_undetermined_key',
                                    self.dll, c_int,
                                    ('change_set', CHANGE_SET, 1),
                                    ('key', c_char_p, 1))

if __name__ == "__main__":
    import gobject
    import dbus
    import dbus.service
    from dbus.mainloop.glib import DBusGMainLoop
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()

    busname = dbus.service.BusName("org.freedesktop.ContextKit.Testing.Provider",
                                    dbus.SessionBus())

    cp = ContextProvider()
    cp.init(["foo.bar", "foo.baz"], 1, None, None, None, None, None, None)
    print cp.no_of_subscribers("foo.bar")
    cs = cp.change_set_create()
    cp.change_set_cancel(cs)
    cs2 = cp.change_set_create()
    cp.change_set_add_int(cs2, "foo.bar", 1)
    cp.change_set_add_undetermined_key(cs2, "foo.baz")
    cp.change_set_commit(cs2)
    loop.run()
