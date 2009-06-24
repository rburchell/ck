import sys

def install_colourer():
    def red(s):
        return '\x1b[31m%s\x1b[0m' % s

    def green(s):
        return '\x1b[32m%s\x1b[0m' % s

    patterns = {
        'handled': green,
        'not handled': red,
        }

    class Colourer:
        def __init__(self, fh, patterns):
            self.fh = fh
            self.patterns = patterns

        def write(self, s):
            f = self.patterns.get(s, lambda x: x)
            self.fh.write(f(s))

    sys.stdout = Colourer(sys.stdout, patterns)
    return sys.stdout


