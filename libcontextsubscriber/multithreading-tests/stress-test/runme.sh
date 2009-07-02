# RunMe & ReadMe
#
# provider.py  will run  flexiprovider and update test.int every second to the POSIX time.
# Additionally, it will change the registry every two seconds.
#
# stress-test takes two int arguments: the number of threads using the first and the second provider.
#
# Compiling: for now, use qmake & make. Sorry.

#xterm -e 'rlwrap ../../../python/context-provide  my.test.provider int test.int 0 int test.int2 0' &
#xterm -e 'rlwrap ../../../python/context-provide  my.test2.provider int test2.int 0 int test2.int2 0' &

xterm -e './provider.py' &
CONTEXT_PROVIDERS=../stress-test/ ./run-test 3 3
