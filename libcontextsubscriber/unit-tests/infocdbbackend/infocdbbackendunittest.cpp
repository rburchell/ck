/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QtTest/QtTest>
#include <QtCore>
#include "fileutils.h"
#include "infocdbbackend.h"
#include "cdbwriter.h"

class InfoCdbBackendUnitTest : public QObject
{
    Q_OBJECT
    InfoCdbBackend *backend;

private slots:
    void initTestCase();
    void listKeys();
    void listPlugins();
    void listKeysForPlugin();
    void typeForKey();
};

void InfoCdbBackendUnitTest::initTestCase()
{
    // FIXME: LOCAL_DIR
    CDBWriter writer("cache.cdb");
    writer.add("KEYS", "Battery.Charging");
    writer.add("KEYS", "Internet.BytesOut");
    writer.add("PLUGINS", "contextkit-dbus");
    writer.add("contextkit-dbus:KEYS", "Battery.Charging");
    writer.add("contextkit-dbus:KEYS", "Internet.BytesOut");
    writer.add("Battery.Charging:TYPE", "TRUTH");
    writer.add("Internet.BytesOut:TYPE", "INTEGER");
    writer.close();

    utilSetEnv("CONTEXT_PROVIDERS", LOCAL_DIR);
    utilSetEnv("CONTEXT_CORE_DECLARATIONS", "/dev/null");
    backend = new InfoCdbBackend();
}

void InfoCdbBackendUnitTest::listKeys()
{
    QStringList keys = backend->listKeys();
    QCOMPARE(keys.count(), 2);
    QVERIFY(keys.contains("Battery.Charging"));
    QVERIFY(keys.contains("Internet.BytesOut"));
}

void InfoCdbBackendUnitTest::listPlugins()
{
    QStringList plugins = backend->listPlugins();
    QCOMPARE(plugins.count(), 1);
    QVERIFY(plugins.contains("contextkit-dbus"));
}

void InfoCdbBackendUnitTest::listKeysForPlugin()
{
    QStringList keys1 = backend->listKeysForPlugin("contextkit-dbus");
    QCOMPARE(keys1.count(), 2);
    QVERIFY(keys1.contains("Battery.Charging"));
    QVERIFY(keys1.contains("Internet.BytesOut"));

    QStringList keys2 = backend->listKeysForPlugin("non-existant");
    QCOMPARE(keys2.count(), 1);
}

void InfoCdbBackendUnitTest::typeForKey()
{
    QCOMPARE(backend->typeForKey("Internet.BytesOut"), QString("INTEGER"));
    QCOMPARE(backend->typeForKey("Battery.Charging"), QString("TRUTH"));
    QCOMPARE(backend->typeForKey("Does.Not.Exist"), QString());
}

#include "infocdbbackendunittest.moc"
QTEST_MAIN(InfoCdbBackendUnitTest);
