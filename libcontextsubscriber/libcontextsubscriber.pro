TEMPLATE = subdirs
SUBDIRS = src unit-tests customer-tests cli cls update-contextkit-providers

unit-tests.depends = src
customer-tests.depends = src
cli.depends = src
cls.depends = src
update-contextkit-providers.depends = src
