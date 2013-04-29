TEMPLATE = subdirs
SUBDIRS = src unit-tests customer-tests

unit-tests.depends = src
customer-tests.depends = src
