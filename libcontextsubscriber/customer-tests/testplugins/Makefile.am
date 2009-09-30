SUBDIRS = timeplugin1 timeplugin2
EXTRA_DIST = timeplugin.cpp timeplugin.h

# they are needed for running ../pluginchanging tests locally
all-local:
	ln -sf timeplugin1/.libs/contextsubscribertime1.so .
	ln -sf timeplugin2/.libs/contextsubscribertime2.so .

clean-local:
	rm -f contextsubscribertime1.so
	rm -f contextsubscribertime2.so
