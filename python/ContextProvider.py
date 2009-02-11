from ctypes import *
from CTypesHelpers import *


_dll = CDLL("libcontextprovider.so")

class STRING_SET(c_void_p):
    pass

class CHANGE_SET(c_void_p):
    pass

class PROVIDER(c_void_p):
    pass


class ContextProvider:
    GET_CALLBACK = CFUNCTYPE(None, STRING_SET, CHANGE_SET, c_void_p)
    SUBSCRIBED_CALLBACK = CFUNCTYPE(None, STRING_SET, c_void_p)
    UNSUBSCRIBED_CALLBACK = CFUNCTYPE(None, STRING_SET, STRING_SET, c_void_p)

    init = cfunc('context_provider_init', _dll, None,
                 ('bus_type', c_int, 1),
                 ('bus_name', c_char_p, 1))
    install = cfunc('context_provider_install', _dll, PROVIDER,
                 ('provided_keys', ListPOINTER (c_char_p), 1),
                 ('get_cb', GET_CALLBACK, 1),
                 ('get_cb_target', c_void_p, 1),
                 ('first_cb', SUBSCRIBED_CALLBACK, 1),
                 ('first_cb_target', c_void_p, 1),
                 ('last_cb', UNSUBSCRIBED_CALLBACK, 1),
                 ('last_cb_target', c_void_p, 1))
    remove = cfunc('context_provider_remove', _dll, None,
                    ('provider', PROVIDER, 1))
    no_of_subscribers = cfunc('context_provider_no_of_subscribers', _dll, c_int,
                              ('key', c_char_p, 1))

    change_set_create = cfunc('context_provider_change_set_create', _dll, CHANGE_SET)
    change_set_commit = cfunc('context_provider_change_set_commit', _dll, None,
                              ('change_set', CHANGE_SET, 1))
    change_set_cancel = cfunc('context_provider_change_set_cancel', _dll, None,
                              ('change_set', CHANGE_SET, 1))
    change_set_add_integer = cfunc('context_provider_change_set_add_integer', _dll, c_int,
                               ('change_set', CHANGE_SET, 1),
                               ('key', c_char_p, 1),
                               ('val', c_int, 1))
    change_set_add_double = cfunc('context_provider_change_set_add_double', _dll, c_int,
                                  ('change_set', CHANGE_SET, 1),
                                  ('key', c_char_p, 1),
                                  ('val', c_double, 1))
    change_set_add_boolean = cfunc('context_provider_change_set_add_boolean', _dll, c_int,
                               ('change_set', CHANGE_SET, 1),
                               ('key', c_char_p, 1),
                               ('val', c_int, 1))
    change_set_add_undetermined_key = cfunc('context_provider_change_set_add_undetermined_key', _dll, c_int,
                                            ('change_set', CHANGE_SET, 1),
                                            ('key', c_char_p, 1))

class StringSet:

    new = cfunc('context_provider_string_set_new', _dll, STRING_SET)
    new_sized = cfunc('context_provider_string_set_new_sized', _dll, STRING_SET,
                      ('size', c_int, 1))
    new_from_array = cfunc('context_provider_string_set_new_from_array', _dll, STRING_SET,
                      ('array', ListPOINTER(c_char_p), 1),
                      ('length', c_int, 1))
    new_intersection = cfunc('context_provider_string_set_new_intersection', _dll, STRING_SET,
                      ('left', STRING_SET, 1),
                      ('right', STRING_SET, 1))
    new_symmetric_difference = cfunc('context_provider_string_set_new_symmetric_difference', _dll, STRING_SET,
                      ('left', STRING_SET, 1),
                      ('right', STRING_SET, 1))
    new_difference = cfunc('context_provider_string_set_new_difference', _dll, STRING_SET,
                      ('left', STRING_SET, 1),
                      ('right', STRING_SET, 1))
    new_union = cfunc('context_provider_string_set_new_union', _dll, STRING_SET,
                      ('left', STRING_SET, 1),
                      ('right', STRING_SET, 1))
    add = cfunc('context_provider_string_set_add', _dll, None,
                ('self', STRING_SET, 1),
                ('element', c_char_p, 1))
    remove = cfunc('context_provider_string_set_remove', _dll, c_int,
                ('self', STRING_SET, 1),
                ('element', c_char_p, 1))
    clear = cfunc('context_provider_string_set_clear', _dll, None,
                ('self', STRING_SET, 1))
    is_member = cfunc('context_provider_string_set_is_member', _dll, c_int,
                ('self', STRING_SET, 1),
                ('element', c_char_p, 1))
    is_equal =  cfunc('context_provider_string_set_is_equal', _dll, c_int,
                      ('left', STRING_SET, 1),
                      ('right', STRING_SET, 1))
    is_subset_of =  cfunc('context_provider_string_set_is_subset_of', _dll, c_int,
                      ('left', STRING_SET, 1),
                      ('right', STRING_SET, 1))
    is_disjoint =  cfunc('context_provider_string_set_is_disjoint', _dll, c_int,
                      ('left', STRING_SET, 1),
                      ('right', STRING_SET, 1))
    size = cfunc('context_provider_string_set_size', _dll, None,
                ('self', STRING_SET, 1))
    debug = cfunc('context_provider_string_set_debug', _dll, c_char_p,
                ('self', STRING_SET, 1))
    ref = cfunc('context_provider_string_set_ref', _dll, c_char_p,
                ('self', STRING_SET, 1))
    unref = cfunc('context_provider_string_set_unref', _dll, c_char_p,
                ('self', STRING_SET, 1))

if __name__ == "__main__":
    import gobject
    import dbus
    import dbus.service
    from dbus.mainloop.glib import DBusGMainLoop
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()

    def py_get_cb (ss, cs, d):
        print "This is get callback"
        #print StringSet.debug(ss)
        return 0
    def py_first_cb (ss, d):
        print StringSet.debug(ss)
    def py_last_cb (ss, ss_remain, d):
        print StringSet.debug(ss)
        print StringSet.debug(ss_remain)


    ContextProvider.init(0, "org.freedesktop.ContextKit.Testing.Provider")
    get_cb = ContextProvider.GET_CALLBACK(py_get_cb)
    subscribe_cb = ContextProvider.SUBSCRIBED_CALLBACK(py_first_cb)
    unsubscribe_cb = ContextProvider.UNSUBSCRIBED_CALLBACK(py_last_cb)
    p =ContextProvider.install(["foo.bar", "foo.baz"],
                         get_cb, None,
                         subscribe_cb, None,
                         unsubscribe_cb, None)
    print ContextProvider.no_of_subscribers("foo.bar")
    cs = ContextProvider.change_set_create()
    ContextProvider.change_set_cancel(cs)
    cs2 = ContextProvider.change_set_create()
    ContextProvider.change_set_add_integer(cs2, "foo.bar", 1)
    ContextProvider.change_set_add_undetermined_key(cs2, "foo.baz")
    ContextProvider.change_set_commit(cs2)
    #ContextProvider.remove (p) # FIXME: Removed for testing purposes (now can be poked with d-feet)
    loop.run()
