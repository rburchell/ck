lib_LTLIBRARIES = libcontextprovider.la

libcontextprovider_la_SOURCES = loggingfeatures.h       \
                                service.h               \
                                service.cpp             \
                                property.h              \
                                property.cpp            \
                                propertyprivate.h       \
                                propertyprivate.cpp     \
                                group.h			\
                                group.cpp		\
                                queuedinvoker.h         \
                                queuedinvoker.cpp       \
                                contextc.h              \
                                contextc.cpp            \
                                listeners.h             \
                                listeners.cpp		\
                                context_provider.h	\
                                servicebackend.h	\
                                servicebackend.cpp      \
                                propertyadaptor.h       \
                                propertyadaptor.cpp


includecontextproviderdir=$(includedir)/contextprovider
includecontextprovider_HEADERS = ContextProvider contextc.h context_provider.h
includecontextprovidercontextdir=$(includecontextproviderdir)/context
includecontextprovidercontext_HEADERS = service.h property.h group.h

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) $(DBUS_CFLAGS) \
	      -I$(top_srcdir)/common			       \
              '-DCONTEXT_LOG_MODULE_NAME="libcontextprovider"'

LIBS += $(QtCore_LIBS) $(QtDBus_LIBS)
libcontextprovider_la_LIBADD=$(top_builddir)/common/libcommon.la

$(top_builddir)/common/libcommon.la:
	$(MAKE) -C $(top_builddir)/common libcommon.la

.PHONY: $(top_builddir)/common/libcommon.la

# moccing
nodist_libcontextprovider_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextprovider_la_SOURCES))
include $(top_srcdir)/am/qt.am

