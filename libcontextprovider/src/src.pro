TARGET = contextprovider
TEMPLATE = lib
QT = core dbus
CONFIG += link_pkgconfig
PKGCONFIG += dbus-1
VERSION = 0.0.0 # force to match to autofoo soversion

HEADERS = \
    loggingfeatures.h \
    service.h \
    property.h \
    propertyprivate.h \
    group.h \
    contextc.h \
    listeners.h \
    context_provider.h \
    servicebackend.h \
    propertyadaptor.h


SOURCES = \
    service.cpp \
    property.cpp \
    propertyprivate.cpp \
    group.cpp \
    contextc.cpp \
    listeners.cpp \
    servicebackend.cpp \
    propertyadaptor.cpp

libcp.path = /usr/include/contextprovider
libcp.files = ContextProvider contextc.h context_provider.h
INSTALLS += libcp

libcp_context.path = $${libcp.path}/context
libcp_context.files = group.h property.h service.h
INSTALLS += libcp_context

libcpc.path = $${libcp.path}/context
libcpc.files = service.h property.h group.h

include(../../common/common.pri)

DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"libcontextpovider\\\"
DEFINES += CONTEXT_LOG_HIDE_DEBUG

dbus_policy.path = /etc/dbus-1/system.d
dbus_policy.files = libcontextprovider0.conf
INSTALLS += dbus_policy

PCFILE=contextprovider-1.0.pc

# substitutions
system(cp $${PCFILE}.in $$PCFILE)
system(sed -i "s/\\@VERSION\\@/$$VERSION/g" $$PCFILE)

pcfiles.files = $$PCFILE
pcfiles.path = $$[QT_INSTALL_LIBS]/pkgconfig
INSTALLS += pcfiles

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

