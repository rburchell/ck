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

private:
    void createBaseDatabase(QString path);
    void createAlternateDatabase(QString path);

private slots:
    void initTestCase();
    void databaseExists();
    void databaseDirectory();
    void listKeys();
    void typeForKey();
    void docForKey();
    void keyDeclared();
    void keyProvided();
    void listProviders();
    void dynamics();
    void removed();
    void cleanupTestCase();
};

void InfoCdbBackendUnitTest::createBaseDatabase(QString path)
{
    CDBWriter writer(path);

    if (! writer.isWritable())
        qFatal("Failed to open %s for reading!", path.toLocal8Bit().data());

    writer.add("KEYS", "Battery.Charging");
    writer.add("KEYS", "Internet.BytesOut");
    writer.add("PLUGINS", "contextkit-dbus");
    writer.add("contextkit-dbus:KEYS", "Battery.Charging");
    writer.add("contextkit-dbus:KEYS", "Internet.BytesOut");
    writer.add("Battery.Charging:KEYTYPE", "TRUTH");
    writer.add("Internet.BytesOut:KEYTYPE", "INTEGER");
    writer.add("Battery.Charging:KEYDOC", "doc1");
    writer.add("Internet.BytesOut:KEYPLUGIN", "contextkit-dbus");
    writer.add("Battery.Charging:KEYPLUGIN", "contextkit-dbus");
    writer.add("Battery.Charging:KEYCONSTRUCTIONSTRING", "system:org.freedesktop.ContextKit.contextd1");
    writer.add("Internet.BytesOut:KEYCONSTRUCTIONSTRING", "session:org.freedesktop.ContextKit.contextd2");
    writer.close();
}

void InfoCdbBackendUnitTest::createAlternateDatabase(QString path)
{
    CDBWriter writer(path);

    if (! writer.isWritable())
        qFatal("Failed to open %s for reading!", path.toLocal8Bit().data());

    writer.add("KEYS", "Battery.Charging");
    writer.add("KEYS", "Battery.Capacity");
    writer.add("PLUGINS", "contextkit-dbus");
    writer.add("contextkit-dbus:KEYS", "Battery.Charging");
    writer.add("contextkit-dbus:KEYS", "Battery.Capacity");
    writer.add("Battery.Charging:KEYTYPE", "INTEGER");
    writer.add("Battery.Charging:KEYDOC", "doc1");
    writer.add("Battery.Charging:KEYPLUGIN", "contextkit-dbus");
    writer.add("Battery.Charging:KEYCONSTRUCTIONSTRING", "system:org.freedesktop.ContextKit.contextd1");
    writer.add("Battery.Capacity:KEYTYPE", "INTEGER");
    writer.add("Battery.Capacity:KEYDOC", "doc3");
    writer.add("Battery.Capacity:KEYPLUGIN", "contextkit-dbus");
    writer.add("Battery.Capacity:KEYCONSTRUCTIONSTRING", "system:org.freedesktop.ContextKit.contextd1");
    writer.close();
}

void InfoCdbBackendUnitTest::initTestCase()
{
    createBaseDatabase("cache.cdb");

    utilSetEnv("CONTEXT_PROVIDERS", "./");
    utilSetEnv("CONTEXT_CORE_DECLARATIONS", "/dev/null");
    backend = new InfoCdbBackend();
}

void InfoCdbBackendUnitTest::databaseDirectory()
{
    QCOMPARE(backend->databaseDirectory(), QString("./"));
}

void InfoCdbBackendUnitTest::databaseExists()
{
    QCOMPARE(backend->databaseExists(), true);
}

void InfoCdbBackendUnitTest::listKeys()
{
    QStringList keys = backend->listKeys();
    QCOMPARE(keys.count(), 2);
    QVERIFY(keys.contains("Battery.Charging"));
    QVERIFY(keys.contains("Internet.BytesOut"));
}

void InfoCdbBackendUnitTest::typeForKey()
{
    QCOMPARE(backend->typeForKey("Internet.BytesOut"), QString("INTEGER"));
    QCOMPARE(backend->typeForKey("Battery.Charging"), QString("TRUTH"));
    QCOMPARE(backend->typeForKey("Does.Not.Exist"), QString());
}

void InfoCdbBackendUnitTest::docForKey()
{
    QCOMPARE(backend->docForKey("Internet.BytesOut"), QString());
    QCOMPARE(backend->docForKey("Battery.Charging"), QString("doc1"));
    QCOMPARE(backend->docForKey("Does.Not.Exist"), QString());
}

void InfoCdbBackendUnitTest::keyDeclared()
{
    foreach (QString key, backend->listKeys())
        QCOMPARE(backend->keyDeclared(key), true);

    QCOMPARE(backend->keyDeclared("Does.Not.Exist"), false);
    QCOMPARE(backend->keyDeclared("Battery.Charging"), true);
}

void InfoCdbBackendUnitTest::keyProvided()
{
    foreach (QString key, backend->listKeys())
        QVERIFY(backend->keyProvided(key) == true);

    QCOMPARE(backend->keyProvided("Does.Not.Exist"), false);
}

void InfoCdbBackendUnitTest::listProviders()
{
    QList <ContextProviderInfo> list1 = backend->listProviders("Battery.Charging");
    QCOMPARE(list1.count(), 1);
    QCOMPARE(list1.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(list1.at(0).constructionString, QString("system:org.freedesktop.ContextKit.contextd1"));

    QList <ContextProviderInfo> list2 = backend->listProviders("Does.Not.Exist");
    QCOMPARE(list2.count(), 0);
}

void InfoCdbBackendUnitTest::dynamics()
{
    backend->connectNotify("-"); // Fake it. Spy does something fishy here.

    // Setup the spy observers
    QSignalSpy spy1(backend, SIGNAL(keysRemoved(QStringList)));
    QSignalSpy spy2(backend, SIGNAL(keysChanged(QStringList)));
    QSignalSpy spy3(backend, SIGNAL(keyChanged(QString)));
    QSignalSpy spy4(backend, SIGNAL(keysAdded(QStringList)));

    createAlternateDatabase("cache-next.cdb");
    QFile::remove("cache.cdb");
    QFile::copy("cache-next.cdb", "cache.cdb");
    QTest::qWait(DEFAULT_WAIT_PERIOD);

    // Test the new values
    QCOMPARE(backend->databaseExists(), true);
    QCOMPARE(backend->listKeys().count(), 2);
    QVERIFY(backend->listKeys().contains("Battery.Charging"));
    QVERIFY(backend->listKeys().contains("Battery.Capacity"));
    QCOMPARE(backend->typeForKey("Battery.Charging"), QString("INTEGER"));
    QCOMPARE(backend->docForKey("Battery.Charging"), QString("doc1"));
    QVERIFY(backend->keyProvided("Battery.Charging") == true);
    QVERIFY(backend->keyProvided("Internet.BytesOut") == false);

    // Test emissions
    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toList().size(), 1);
    QCOMPARE(args1.at(0).toStringList().at(0), QString("Internet.BytesOut"));

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toList().size(), 2);
    QCOMPARE(args2.at(0).toStringList().at(0), QString("Battery.Charging"));
    QCOMPARE(args2.at(0).toStringList().at(1), QString("Battery.Capacity"));

    QCOMPARE(spy3.count(), 3);

    QCOMPARE(spy4.count(), 1);
    QList<QVariant> args4 = spy4.takeFirst();
    QCOMPARE(args4.at(0).toList().size(), 1);
    QCOMPARE(args4.at(0).toStringList().at(0), QString("Battery.Capacity"));

    backend->disconnectNotify("-"); // Fake it. Spy does something fishy here.
}

void InfoCdbBackendUnitTest::removed()
{
    backend->connectNotify("-"); // Fake it. Spy does something fishy here.
    QSignalSpy spy(backend, SIGNAL(keysRemoved(QStringList)));

    QFile::remove("cache.cdb");

    QTest::qWait(DEFAULT_WAIT_PERIOD);

    //QCOMPARE(spy.count(), 1);
    //QList<QVariant> args = spy.takeFirst();
    //QCOMPARE(args.at(0).toList().size(), 2);

    backend->disconnectNotify("-"); // Fake it. Spy does something fishy here.
}

void InfoCdbBackendUnitTest::cleanupTestCase()
{
     QFile::remove("cache.cdb");
     QFile::remove("cache-next.cdb");
}

#include "infocdbbackendunittest.moc"
QTEST_MAIN(InfoCdbBackendUnitTest);
