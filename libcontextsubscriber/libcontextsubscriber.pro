TEMPLATE = subdirs
SUBDIRS = src unit-tests customer-tests cli cls update-contextkit-providers

unit-tests.depends = src
customer-tests.depends = src
cli.depends = src
cls.depends = src
update-contextkit-providers.depends = src

# QT5-TODO
equals(QT_MAJOR_VERSION, 5): SUBDIRS -= unit-tests customer-tests
