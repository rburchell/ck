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
#include "infoxmlbackend.h"

class InfoXmlBackendUnitTest : public QObject
{
    Q_OBJECT
    InfoXmlBackend *backend;

private slots:
    void initTestCase();
    void paths();
    void listKeys();
    void listPlugins();
    void listKeysForPlugin();
    void typeForKey();
    void docForKey();
    void constructionStringForKey();
    void pluginForKey();
    void keyExists();
    void keyProvided();
    void dynamics();
    void cleanupTestCase();
};

void InfoXmlBackendUnitTest::initTestCase()
{
    utilCopyLocalWithRemove("providers2v1.src", "providers2.context");
    utilSetEnv("CONTEXT_PROVIDERS", LOCAL_DIR);
    utilSetEnv("CONTEXT_CORE_DECLARATIONS", "/dev/null");
    backend = new InfoXmlBackend();
}

void InfoXmlBackendUnitTest::listKeys()
{
    QStringList keys = backend->listKeys();
    QCOMPARE(keys.count(), 9);
    QVERIFY(keys.contains("Battery.ChargePercentage"));
    QVERIFY(keys.contains("Battery.LowBattery"));
    QVERIFY(keys.contains("Key.With.Attribute"));
    QVERIFY(keys.contains("Key.With.bool"));
    QVERIFY(keys.contains("Key.With.int32"));
    QVERIFY(keys.contains("Key.With.string"));
    QVERIFY(keys.contains("Key.With.double"));
    QVERIFY(keys.contains("Key.With.complex"));
    QVERIFY(keys.contains("Battery.Charging"));
}

void InfoXmlBackendUnitTest::listPlugins()
{
    QStringList plugins = backend->listPlugins();
    QCOMPARE(plugins.count(), 1);
    QVERIFY(plugins.contains("contextkit-dbus"));
}

void InfoXmlBackendUnitTest::listKeysForPlugin()
{
    QStringList keys = backend->listKeysForPlugin("contextkit-dbus");
    QVERIFY(keys.contains("Battery.ChargePercentage"));
    QVERIFY(keys.contains("Battery.LowBattery"));
    QVERIFY(keys.contains("Key.With.Attribute"));
    QVERIFY(keys.contains("Key.With.bool"));
    QVERIFY(keys.contains("Key.With.int32"));
    QVERIFY(keys.contains("Key.With.string"));
    QVERIFY(keys.contains("Key.With.double"));
    QVERIFY(keys.contains("Key.With.complex"));
    QVERIFY(keys.contains("Battery.Charging"));
    QVERIFY(backend->listKeysForPlugin("does-not-exist").count() == 0);
}

void InfoXmlBackendUnitTest::typeForKey()
{
    QCOMPARE(backend->typeForKey("Battery.ChargePercentage"), QString());
    QCOMPARE(backend->typeForKey("Key.With.Attribute"), QString("TRUTH"));
    QCOMPARE(backend->typeForKey("Battery.LowBattery"), QString("TRUTH"));
    QCOMPARE(backend->typeForKey("Key.With.bool"), QString("TRUTH"));
    QCOMPARE(backend->typeForKey("Key.With.int32"), QString("INT"));
    QCOMPARE(backend->typeForKey("Key.With.string"), QString("STRING"));
    QCOMPARE(backend->typeForKey("Key.With.double"), QString("DOUBLE"));
    QCOMPARE(backend->typeForKey("Key.With.complex"), QString());
    QCOMPARE(backend->typeForKey("Battery.Charging"), QString("TRUTH"));
    QCOMPARE(backend->typeForKey("Does.Not.Exist"), QString());
}

void InfoXmlBackendUnitTest::docForKey()
{
    QCOMPARE(backend->docForKey("Battery.ChargePercentage"), QString());
    QCOMPARE(backend->docForKey("Battery.LowBattery"), QString("This is true when battery is low"));
    QCOMPARE(backend->docForKey("Key.With.bool"), QString());
    QCOMPARE(backend->docForKey("Key.With.int32"), QString());
    QCOMPARE(backend->docForKey("Key.With.string"), QString());
    QCOMPARE(backend->docForKey("Key.With.double"), QString());
    QCOMPARE(backend->docForKey("Key.With.complex"), QString());
    QCOMPARE(backend->docForKey("Battery.Charging"), QString("This is true when battery is charging"));
    QCOMPARE(backend->docForKey("Does.Not.Exist"), QString());
}

void InfoXmlBackendUnitTest::pluginForKey()
{
    foreach (QString key, backend->listKeys())
        QCOMPARE(backend->pluginForKey(key), QString("contextkit-dbus"));

    QCOMPARE(backend->pluginForKey("Does.Not.Exist"), QString());
}

void InfoXmlBackendUnitTest::keyExists()
{
    foreach (QString key, backend->listKeys())
        QCOMPARE(backend->keyExists(key), true);

    QCOMPARE(backend->keyExists("Does.Not.Exist"), false);
    QCOMPARE(backend->keyExists("Battery.Charging"), true);
}

void InfoXmlBackendUnitTest::paths()
{
    QCOMPARE(InfoXmlBackend::registryPath(), QString(LOCAL_DIR));
    QCOMPARE(InfoXmlBackend::coreDeclPath(), QString("/dev/null"));
}

void InfoXmlBackendUnitTest::constructionStringForKey()
{
    foreach (QString key, backend->listKeys())
        QVERIFY(backend->constructionStringForKey(key) != QString());

    QCOMPARE(backend->constructionStringForKey("Battery.Charging"), QString("system:org.freedesktop.ContextKit.contextd2"));
    QCOMPARE(backend->constructionStringForKey("Key.With.bool"), QString("session:org.freedesktop.ContextKit.contextd1"));
    QCOMPARE(backend->constructionStringForKey("Does.Not.Exist"), QString());
}

void InfoXmlBackendUnitTest::keyProvided()
{
    foreach (QString key, backend->listKeys())
        QVERIFY(backend->keyProvided(key) == true);

    QCOMPARE(backend->keyProvided("Does.Not.Exist"), false);
}

void InfoXmlBackendUnitTest::dynamics()
{
    QStringList keys = backend->listKeys();
    QCOMPARE(keys.count(), 9);
    QVERIFY(keys.contains("Battery.Charging"));
    QCOMPARE(backend->keyExists("System.Active"), false);
    
    utilCopyLocalWithRemove("providers2v2.src", "providers2.context");
    utilCopyLocalWithRemove("providers3.src", "providers3.context");
    
    QCOMPARE(backend->keyExists("System.Active"), true);
    QCOMPARE(backend->typeForKey("Battery.Charging"), QString("INTEGER"));
}

void InfoXmlBackendUnitTest::cleanupTestCase()
{
     QFile::remove("providers3.context");
     QFile::remove("providers2.context");
     QFile::remove(LOCAL_FILE("providers3.context"));
     QFile::remove(LOCAL_FILE("providers2.context"));
}

#include "infoxmlbackendunittest.moc"
QTEST_MAIN(InfoXmlBackendUnitTest);
