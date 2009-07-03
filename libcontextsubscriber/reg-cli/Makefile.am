bin_PROGRAMS = context-print-info
context_print_info_SOURCES = print-info.cpp

AM_CXXFLAGS = $(QtCore_CFLAGS) $(QtDBus_CFLAGS)
LIBS += $(QtCore_LIBS)

# library dependency hack for seamless make in cli/
AM_CXXFLAGS += -I$(srcdir)/../src
context_print_info_LDADD = ../src/libcontextsubscriber.la
../src/libcontextsubscriber.la:
	$(MAKE) -C ../src libcontextsubscriber.la
.PHONY: ../src/libcontextsubscriber.la

# moccing not needed
include $(top_srcdir)/am/qt.am
