from ctypes import *
from CTypesHelpers import *


class STRING_SET(c_void_p):
    pass

class CHANGE_SET(c_void_p):
    pass

class ContextProvider:
    _dll = CDLL("libcontextprovider.so")
    STRING_ARRAY = POINTER(c_char_p)
    GET_CALLBACK = POINTER(CFUNCTYPE(STRING_SET, CHANGE_SET, c_void_p))
    SUBSCRIBE_CALLBACK = POINTER(CFUNCTYPE(STRING_SET, c_void_p))

    init = cfunc('context_provider_init', _dll, None,
                 ('provided_keys', ListPOINTER (c_char_p), 1),
                 ('useSessionBus', c_int, 1),
                 ('get_cb', GET_CALLBACK, 1),
                 ('get_cb_target', c_void_p, 1),
                 ('first_cb', SUBSCRIBE_CALLBACK, 1),
                 ('first_cb_target', c_void_p, 1),
                 ('last_cb', SUBSCRIBE_CALLBACK, 1),
                 ('last_cb_target', c_void_p, 1))
    no_of_subscribers = cfunc('context_provider_no_of_subscribers', _dll, c_int,
                              ('key', c_char_p, 1))

    change_set_create = cfunc('context_provider_change_set_create', _dll, CHANGE_SET)
    change_set_commit = cfunc('context_provider_change_set_commit', _dll, None,
                              ('change_set', CHANGE_SET, 1))
    change_set_cancel = cfunc('context_provider_change_set_cancel', _dll, None,
                              ('change_set', CHANGE_SET, 1))
    change_set_add_int = cfunc('context_provider_change_set_add_int', _dll, c_int,
                               ('change_set', CHANGE_SET, 1),
                               ('key', c_char_p, 1),
                               ('val', c_int, 1))
    change_set_add_double = cfunc('context_provider_change_set_add_double', _dll, c_int,
                                  ('change_set', CHANGE_SET, 1),
                                  ('key', c_char_p, 1),
                                  ('val', c_double, 1))

    change_set_add_undetermined_key = cfunc('context_provider_change_set_add_undetermined_key', _dll, c_int,
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

    ContextProvider.init(["foo.bar", "foo.baz"], 1, None, None, None, None, None, None)
    print ContextProvider.no_of_subscribers("foo.bar")
    cs = ContextProvider.change_set_create()
    ContextProvider.change_set_cancel(cs)
    cs2 = ContextProvider.change_set_create()
    ContextProvider.change_set_add_int(cs2, "foo.bar", 1)
    ContextProvider.change_set_add_undetermined_key(cs2, "foo.baz")
    ContextProvider.change_set_commit(cs2)
    loop.run()
