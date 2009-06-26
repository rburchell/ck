#xterm -e 'rlwrap ../../../python/context-provide  my.test.provider int test.int 0 int test.int2 0' &
#xterm -e 'rlwrap ../../../python/context-provide  my.test2.provider int test2.int 0 int test2.int2 0' &
xterm -e './provider.py' &
CONTEXT_PROVIDERS=../stress-test/ ./stress-test 3 3
