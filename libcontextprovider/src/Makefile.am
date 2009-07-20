lib_LTLIBRARIES = libcontextprovider.la

libcontextprovider_la_SOURCES = logging.h		\
				logging.cpp		\
				loggingfeatures.h	\
				context.h		\
				context.cpp             \
				manager.h		\
				manager.cpp		\
				manageradaptor.h        \
				manageradaptor.cpp      \
				signalgrouper.h		\
				signalgrouper.cpp	\
				subscriber.cpp		\
				subscriber.h		\
				subscriberadaptor.h	\
				subscriberadaptor.cpp   \
				queuedinvoker.h		\
				queuedinvoker.cpp	\
                                sconnect.h              \
                                contextc.h      	\
                                contextc.cpp

includecontextproviderdir=$(includedir)/contextprovider
includecontextprovider_HEADERS = context.h contextc.h signalgrouper.h
AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)		\
	      '-DCONTEXT_LOG_MODULE_NAME="libcontextprovider"'
LIBS += $(QtCore_LIBS) $(QtDBus_LIBS)

# moccing
nodist_libcontextprovider_la_SOURCES = mocs.cpp
QT_TOMOC = $(filter %.h, $(libcontextprovider_la_SOURCES))
include $(top_srcdir)/am/qt.am

