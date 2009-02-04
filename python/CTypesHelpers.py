# From http://wwwx.cs.unc.edu/~gb/wp/blog/2007/02/11/ctypes-tricks/

from ctypes import *

def cfunc(name, dll, result, *args):
    '''build and apply a ctypes prototype complete with parameter flags'''
    atypes = []
    aflags = []
    for arg in args:
        atypes.append(arg[1])
        aflags.append((arg[2], arg[0]) + arg[3:])
    return CFUNCTYPE(result, *atypes)((name, dll), tuple(aflags))

class ListPOINTER(object):
    '''Just like a POINTER but accept a list of ctype as an argument'''
    def __init__(self, etype):
        self.etype = etype

    def from_param(self, param):
        if isinstance(param, (list,tuple)):
            return (self.etype * len(param))(*param)

class ListPOINTER2(object):
    '''Just like POINTER(POINTER(ctype)) but accept a list of lists of ctype'''
    def __init__(self, etype):
        self.etype = etype

    def from_param(self, param):
        if isinstance(param, (list,tuple)):
            val = (POINTER(self.etype) * len(param))()
            for i,v in enumerate(param):
                if isinstance(v, (list,tuple)):
                    val[i] = (self.etype * len(v))(*v)
                else:
                    raise TypeError, 'nested list or tuple required at %d' % i
            return val
        else:
            raise TypeError, 'list or tuple required'


class ByRefArg(object):
    '''Just like a POINTER but accept an argument and pass it byref'''
    def __init__(self, atype):
        self.atype = atype

    def from_param(self, param):
        return byref(self.atype(param))

# hack the ctypes.Structure class to include printing the fields
class _Structure(Structure):
    def __repr__(self):
        '''Print the fields'''
        res = []
        for field in self._fields_:
            res.append('%s=%s' % (field[0], repr(getattr(self, field[0]))))
        return self.__class__.__name__ + '(' + ','.join(res) + ')'
    @classmethod
    def from_param(cls, obj):
        '''Magically construct from a tuple'''
        if isinstance(obj, cls):
            return obj
        if isinstance(obj, tuple):
            return cls(*obj)
        raise TypeError

