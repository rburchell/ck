xterm -e 'rlwrap ../../../python/context-provide  my.test.provider int test.int 1' &
CONTEXT_PROVIDERS=. ./stress-test 5
