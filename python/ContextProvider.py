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
        self.GET_CALLBACK = CFUNCTYPE(STRING_SET, CHANGE_SET, c_void_p)
        self.SUBSCRIBE_CALLBACK = CFUNCTYPE(STRING_SET, c_void_p)

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

        self.change_set_create = ('context_provider_change_set_create', self.dll, CHANGE_SET,
                                    None);


if __name__ == "__main__":
    cp = ContextProvider()
