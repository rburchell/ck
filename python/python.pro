TEMPLATE = aux

rlwrap.files = context-rlwrap
rlwrap.path = /usr/bin/
INSTALLS += rlwrap

pyfiles.files = ContextKit/flexiprovider.py \
                   ContextKit/ContextProvider.py \
                   ContextKit/CTypesHelpers.py ContextKit/__init__.py \
                   ContextKit/cltool.py
pyfiles.path = /usr/lib/python2.7/site-packages/ContextKit
INSTALLS += pyfiles
