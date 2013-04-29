include(../../test.pri)
TARGET =  waitforsubscriptiontests

testdata.path = /usr/share/libcontextsubscriber-tests/waitforsubscription
testdata.files = context-provide.context
INSTALLS += testdata

SOURCES = testwaitforsubscription.cpp
HEADERS = testwaitforsubscription.h
