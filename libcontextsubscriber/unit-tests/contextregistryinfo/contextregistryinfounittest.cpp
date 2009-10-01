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
#include "contextregistryinfo.h"
#include "fileutils.h"
#include "infobackend.h"

/* Mocked infobackend */

InfoBackend* currentBackend = NULL;

InfoBackend* InfoBackend::instance(const QString &backendName)
{
    if (currentBackend)
        return currentBackend;
    else {
        currentBackend = new InfoBackend();
        return currentBackend;
    }
}

QString InfoBackend::name() const
{
    return "test";
}

QStringList InfoBackend::listKeys() const
{
    QStringList l;
    l << QString("Battery.Charging");
    l << QString("Media.NowPlaying");
    return l;
}

QStringList InfoBackend::listKeysForPlugin(QString plugin) const
{
    if (plugin == "contextkit-dbus") {
        QStringList l;
        l << QString("Battery.Charging");
        l << QString("Media.NowPlaying");
        return l;
    } else
        return QStringList();
}

QStringList InfoBackend::listPlugins() const
{
    QStringList l;
    l << QString("contextkit-dbus");
    return l;
}

QString InfoBackend::constructionStringForKey(QString key) const
{
    if (key == "Battery.Charging")
        return "system:org.freedesktop.ContextKit.contextd";
    else if (key == "Media.NowPlaying")
        return "system:com.nokia.musicplayer";
    return QString();
}

void InfoBackend::fireKeysChanged(const QStringList& keys)
{
    emit keysChanged(keys);
}

void InfoBackend::fireKeysAdded(const QStringList& keys)
{
    emit keysAdded(keys);
}

void InfoBackend::fireKeysRemoved(const QStringList& keys)
{
    emit keysRemoved(keys);
}

/* ContextRegistryInfoUnitTest */

class ContextRegistryInfoUnitTest : public QObject
{
    Q_OBJECT
    ContextRegistryInfo *registry;

private:
    ContextRegistryInfo *context;

private slots:
    void initTestCase();
    void listKeys();
    void listKeysForPlugin();
    void listProviders();
    void listPlugins();
    void backendName();
    void signalling();
};

void ContextRegistryInfoUnitTest::initTestCase()
{
    registry = ContextRegistryInfo::instance();
}

void ContextRegistryInfoUnitTest::listKeys()
{
    QStringList keys1 = registry->listKeys();
    QCOMPARE(keys1.count(), 2);
    QVERIFY(keys1.contains("Battery.Charging"));
    QVERIFY(keys1.contains("Media.NowPlaying"));

    QStringList keys2 = registry->listKeys("org.freedesktop.ContextKit.contextd");
    QCOMPARE(keys2.count(), 1);
    QVERIFY(keys2.contains("Battery.Charging"));

    QStringList keys3 = registry->listKeys("com.nokia.musicplayer");
    QCOMPARE(keys3.count(), 1);
    QVERIFY(keys3.contains("Media.NowPlaying"));

    QStringList keys4 = registry->listKeys("com.something");
    QCOMPARE(keys4.count(), 0);
}

void ContextRegistryInfoUnitTest::listKeysForPlugin()
{
    QStringList keys1 = registry->listKeysForPlugin("contextkit-dbus");
    QCOMPARE(keys1.count(), 2);
    QVERIFY(keys1.contains("Battery.Charging"));
    QVERIFY(keys1.contains("Media.NowPlaying"));

    QStringList keys2 = registry->listKeysForPlugin("does-not-exist");
    QCOMPARE(keys2.count(), 0);
}

void ContextRegistryInfoUnitTest::listProviders()
{
    QStringList providers = registry->listProviders();
    QCOMPARE(providers.count(), 2);
    QVERIFY(providers.contains("com.nokia.musicplayer"));
    QVERIFY(providers.contains("org.freedesktop.ContextKit.contextd"));
}

void ContextRegistryInfoUnitTest::listPlugins()
{
    QStringList plugins = registry->listPlugins();
    QCOMPARE(plugins.count(), 1);
    QVERIFY(plugins.contains("contextkit-dbus"));
}

void ContextRegistryInfoUnitTest::backendName()
{
    QCOMPARE(registry->backendName(), QString("test"));
}

void ContextRegistryInfoUnitTest::signalling()
{
    QSignalSpy spy1(registry, SIGNAL(keysChanged(QStringList)));
    QSignalSpy spy2(registry, SIGNAL(keysAdded(QStringList)));
    QSignalSpy spy3(registry, SIGNAL(keysRemoved(QStringList)));

    QStringList keys;
    keys << QString("Battery.Charging");
    keys << QString("Media.NowPlaying");

    currentBackend->fireKeysChanged(keys);
    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toStringList(), keys);

    currentBackend->fireKeysAdded(keys);
    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toStringList(), keys);

    currentBackend->fireKeysRemoved(keys);
    QCOMPARE(spy3.count(), 1);
    QList<QVariant> args3 = spy3.takeFirst();
    QCOMPARE(args3.at(0).toStringList(), keys);
}

#include "contextregistryinfounittest.moc"
QTEST_MAIN(ContextRegistryInfoUnitTest);
