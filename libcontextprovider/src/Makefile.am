lib_LTLIBRARIES = libcontextprovider.la

libcontextprovider_la_SOURCES = loggingfeatures.h       \
                                context.h               \
                                context.cpp             \
                                manager.h               \
                                manager.cpp             \
                                manageradaptor.h        \
                                manageradaptor.cpp      \
                                contextgroup.h		\
                                contextgroup.cpp	\
                                subscriber.cpp          \
                                subscriber.h            \
                                subscriberadaptor.h     \
                                subscriberadaptor.cpp   \
                                queuedinvoker.h         \
                                queuedinvoker.cpp       \
                                contextc.h              \
                                contextc.cpp            \
                                listeners.h             \
                                listeners.cpp		\
				context_provider.h

includecontextproviderdir=$(includedir)/contextprovider
includecontextprovider_HEADERS = context.h contextc.h contextgroup.h context_provider.h
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS) $(DBUS_CFLAGS) \
	      -I$(srcdir)/../../common			       \
              '-DCONTEXT_LOG_MODULE_NAME="libcontextprovider"'

LIBS += $(QtCore_LIBS) $(QtDBus_LIBS) 
libcontextprovider_la_LIBADD=../../common/libcommon.la

../../common/libcommon.la:
	$(MAKE) -C ../../common libcommon.la

.PHONY: ../../common/libcommon.la

# moccing
nodist_libcontextprovider_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextprovider_la_SOURCES))
include $(top_srcdir)/am/qt.am

