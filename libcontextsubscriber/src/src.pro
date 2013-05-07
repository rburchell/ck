QT = core xml dbus
TEMPLATE = lib
equals(QT_MAJOR_VERSION, 4): TARGET = contextsubscriber
equals(QT_MAJOR_VERSION, 5): TARGET = contextsubscriber5
VERSION = 0.0.0 # force to match to autofoo soversion

SOURCES = contextproperty.cpp \
          propertyhandle.cpp \
          provider.cpp \
          subscriberinterface.cpp \
          contextpropertyinfo.cpp \
          contextregistryinfo.cpp \
          infobackend.cpp \
          infoxmlbackend.cpp \
          cdbwriter.cpp cdbreader.cpp \
          infocdbbackend.cpp \
          dbusnamelistener.cpp handlesignalrouter.cpp \
          queuedinvoker.cpp \
          contextkitplugin.cpp \
          nanoxml.cpp \
          asyncdbusinterface.cpp \
          contexttypeinfo.cpp \
          contexttyperegistryinfo.cpp \
          assoctree.cpp \
          duration.cpp

HEADERS = queuedinvoker.h \
          handlesignalrouter.h \
          contexttypeinfo.h \
          timedvalue.h \
          iproviderplugin.h \
          contextproviderinfo.h \
          nanoxml.h \
          loggingfeatures.h \
          contextkitplugin.h \
          duration.h  \
          contexttyperegistryinfo.h \
          assoctree.h \
          dbusnamelistener.h \
          cdbreader.h \
          infocdbbackend.h \
          infoxmlbackend.h \
          contextproperty.h \
          propertyhandle.h \
          provider.h \
          safedbuspendingcallwatcher.h \
          subscriberinterface.h \
          contextpropertyinfo.h \
          contextregistryinfo.h \
          infobackend.h \
          infokeydata.h \
          cdbwriter.h

equals(QT_MAJOR_VERSION, 4): libcs.path = /usr/include/contextsubscriber
equals(QT_MAJOR_VERSION, 5): libcs.path = /usr/include/contextsubscriber5
libcs.files = \
    contextpropertyinfo.h contextregistryinfo.h iproviderplugin.h \
    contextproviderinfo.h asyncdbusinterface.h timedvalue.h \
    contexttypeinfo.h contextproperty.h \
    contexttyperegistryinfo.h assoctree.h duration.h contextjson.h
INSTALLS += libcs

include(../../common/common.pri)

equals(QT_MAJOR_VERSION, 4): DEFINES += DEFAULT_CONTEXT_SUBSCRIBER_PLUGINS=\\\"/usr/lib/contextkit/subscriber-plugins\\\"
equals(QT_MAJOR_VERSION, 5): DEFINES += DEFAULT_CONTEXT_SUBSCRIBER_PLUGINS=\\\"/usr/lib/contextkit/subscriber-plugins5\\\"
DEFINES += DEFAULT_CONTEXT_PROVIDERS=\\\"/usr/share/contextkit/providers/\\\"
DEFINES += DEFAULT_CONTEXT_CORE_DECLARATIONS=\\\"/usr/share/contextkit/core.context\\\"
DEFINES += DEFAULT_CONTEXT_TYPES=\\\"/usr/share/contextkit/types/\\\"
DEFINES += DEFAULT_CONTEXT_CORE_TYPES=\\\"/usr/share/contextkit/types/core.types\\\"
DEFINES += CONTEXT_LOG_MODULE_NAME=\\\"libcontextsubscriber\\\"
DEFINES += CONTEXT_LOG_HIDE_DEBUG

CONFIG += link_pkgconfig
PKGCONFIG += libcdb

equals(QT_MAJOR_VERSION, 4): PCFILE=contextsubscriber-1.0.pc
equals(QT_MAJOR_VERSION, 5): PCFILE=contextsubscriber5.pc

# substitutions
system(cp $${PCFILE}.in $$PCFILE)
system(sed -i "s/\\@VERSION\\@/$$VERSION/g" $$PCFILE)

pcfiles.files = $$PCFILE
pcfiles.path = $$[QT_INSTALL_LIBS]/pkgconfig
INSTALLS += pcfiles

target.path = $$[QT_INSTALL_LIBS]
INSTALLS += target

