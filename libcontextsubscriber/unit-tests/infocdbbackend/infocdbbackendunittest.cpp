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
    void name();
    void databaseExists();
    void databaseDirectory();
    void listKeys();
    void typeInfoForKey();
    void docForKey();
    void keyDeclared();
    void providersForKey();
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
    writer.add("Battery.Charging:KEYTYPEINFO", ContextTypeInfo::boolType().tree());
    writer.add("Internet.BytesOut:KEYTYPEINFO", ContextTypeInfo::int64Type().tree());
    writer.add("Battery.Charging:KEYDOC", "doc1");

    QVariantList providers1;
    QHash <QString, QVariant> provider1;
    provider1.insert("plugin", "contextkit-dbus");
    provider1.insert("constructionString", "system:org.freedesktop.ContextKit.contextd1");
    providers1 << QVariant(provider1);
    writer.add("Battery.Charging:PROVIDERS", QVariant(providers1));

    QVariantList providers2;
    QHash <QString, QVariant> provider2;
    provider2.insert("plugin", "contextkit-dbus");
    provider2.insert("constructionString", "session:org.freedesktop.ContextKit.contextd2");
    providers2 << QVariant(provider2);
    writer.add("Internet.BytesOut:PROVIDERS", QVariant(providers2));

    writer.close();
}

void InfoCdbBackendUnitTest::createAlternateDatabase(QString path)
{
    CDBWriter writer(path);

    if (! writer.isWritable())
        qFatal("Failed to open %s for reading!", path.toLocal8Bit().data());

    writer.add("KEYS", "Battery.Charging");
    writer.add("KEYS", "Battery.Capacity");
    writer.add("Battery.Charging:KEYTYPEINFO", ContextTypeInfo::int64Type().tree());
    writer.add("Battery.Charging:KEYDOC", "doc1");
    writer.add("Battery.Capacity:KEYTYPEINFO", ContextTypeInfo::int64Type().tree());
    writer.add("Battery.Capacity:KEYDOC", "doc3");

    QVariantList providers1;
    QHash <QString, QVariant> provider1;
    provider1.insert("plugin", "contextkit-dbus");
    provider1.insert("constructionString", "system:org.freedesktop.ContextKit.contextd1");
    providers1 << QVariant(provider1);
    writer.add("Battery.Charging:PROVIDERS", QVariant(providers1));

    QVariantList providers2;
    QHash <QString, QVariant> provider2;
    QHash <QString, QVariant> provider3;
    provider2.insert("plugin", "contextkit-dbus");
    provider2.insert("constructionString", "system:org.freedesktop.ContextKit.contextd1");
    provider3.insert("plugin", "contextkit-dbus");
    provider3.insert("constructionString", "system:org.freedesktop.ContextKit.contextdX");
    providers2 << QVariant(provider2);
    providers2 << QVariant(provider3);
    writer.add("Battery.Capacity:PROVIDERS", providers2);

    writer.close();
}

void InfoCdbBackendUnitTest::initTestCase()
{
    createBaseDatabase("cache.cdb");

    utilSetEnv("CONTEXT_PROVIDERS", "./");
    utilSetEnv("CONTEXT_CORE_DECLARATIONS", "/dev/null");
    backend = new InfoCdbBackend();
}

void InfoCdbBackendUnitTest::name()
{
    QCOMPARE(backend->name(), QString("cdb"));
}

void InfoCdbBackendUnitTest::databaseDirectory()
{
    QVERIFY(backend->databaseDirectory() == QString("./") ||
            backend->databaseDirectory() == QString("."));
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

void InfoCdbBackendUnitTest::typeInfoForKey()
{
    QCOMPARE(backend->typeInfoForKey("Internet.BytesOut").name(), QString("int64"));
    QCOMPARE(backend->typeInfoForKey("Battery.Charging").name(), QString("bool"));
    QCOMPARE(backend->typeInfoForKey("Does.Not.Exist").name(), QString());
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

void InfoCdbBackendUnitTest::providersForKey()
{
    QList <ContextProviderInfo> list1 = backend->providersForKey("Battery.Charging");
    QCOMPARE(list1.count(), 1);
    QCOMPARE(list1.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(list1.at(0).constructionString, QString("system:org.freedesktop.ContextKit.contextd1"));

    QList <ContextProviderInfo> list2 = backend->providersForKey("Does.Not.Exist");
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
    QCOMPARE(backend->typeInfoForKey("Battery.Charging").name(), QString("int64"));
    QCOMPARE(backend->docForKey("Battery.Charging"), QString("doc1"));

    // Check providers
    QList <ContextProviderInfo> list1 = backend->providersForKey("Battery.Charging");
    QCOMPARE(list1.count(), 1);
    QCOMPARE(list1.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(list1.at(0).constructionString, QString("system:org.freedesktop.ContextKit.contextd1"));
    QList <ContextProviderInfo> list2 = backend->providersForKey("Battery.Capacity");
    QCOMPARE(list2.count(), 2);
    QCOMPARE(list2.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(list2.at(0).constructionString, QString("system:org.freedesktop.ContextKit.contextd1"));
    QCOMPARE(list2.at(1).plugin, QString("contextkit-dbus"));
    QCOMPARE(list2.at(1).constructionString, QString("system:org.freedesktop.ContextKit.contextdX"));

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

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toList().size(), 2);

    backend->disconnectNotify("-"); // Fake it. Spy does something fishy here.
}

void InfoCdbBackendUnitTest::cleanupTestCase()
{
    QFile::remove("cache.cdb");
    QFile::remove("cache-next.cdb");
}

#include "infocdbbackendunittest.moc"
QTEST_MAIN(InfoCdbBackendUnitTest);
