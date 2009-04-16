from ctypes import *
from CTypesHelpers import *


_dll = CDLL("libcontextprovider.so")

class ContextProvider:
    SUBSCRIPTION_CHANGED_CALLBACK = CFUNCTYPE(None, c_int, c_void_p)

    init = cfunc('context_provider_init', _dll, None,
                 ('bus_type', c_int, 1),
                 ('bus_name', c_char_p, 1))
    install_group = cfunc('context_provider_install_group', _dll, None,
                 ('key_group', ListPOINTER (c_char_p), 1),
                 ('clear_values_on_subscribe', c_int, 1),
                 ('subscription_changed_cb', SUBSCRIPTION_CHANGED_CALLBACK, 1),
                 ('last_cb_target', c_void_p, 1))
    install_key = cfunc('context_provider_install_key', _dll, None,
                 ('key', c_char_p, 1),
                 ('clear_values_on_subscribe', c_int, 1),
                 ('subscription_changed_cb', SUBSCRIPTION_CHANGED_CALLBACK, 1),
                 ('last_cb_target', c_void_p, 1))

    set_integer = cfunc('context_provider_set_integer', _dll, None,
                               ('key', c_char_p, 1),
                               ('val', c_int, 1))
    set_double = cfunc('context_provider_set_double', _dll, None,
                                  ('key', c_char_p, 1),
                                  ('val', c_double, 1))
    set_boolean = cfunc('context_provider_set_boolean', _dll, None,
                               ('key', c_char_p, 1),
                               ('val', c_int, 1))
    set_string = cfunc('context_provider_set_string', _dll, None,
                               ('key', c_char_p, 1),
                               ('val', c_char_p, 1))
    set_null = cfunc('context_provider_set_null', _dll, None,
                               ('key', c_char_p, 1))

if __name__ == "__main__":
    import gobject
    import dbus
    import dbus.service
    from dbus.mainloop.glib import DBusGMainLoop
    DBusGMainLoop(set_as_default=True)
    loop = gobject.MainLoop()

    def py_subscription_changed_cb (subscribe, d):
        if subscribe:
            print "group subscribed"
        else:
            print "group unsubscribed"


    ContextProvider.init(0, "org.freedesktop.ContextKit.Testing.Provider")
    subscription_changed_cb = ContextProvider.SUBSCRIPTION_CHANGED_CALLBACK(py_subscription_changed_cb)
    p = ContextProvider.install_group(["foo.bar", "foo.baz"], 1,
                                      subscription_changed_cb, None)
    ContextProvider.set_integer("foo.bar", 1)
    ContextProvider.set_null("foo.baz")
    loop.run()
