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
#include "contextpropertyinfo.h"
#include "infobackend.h"

QMap <QString, QString> constructionStringMap;
QMap <QString, QString> typeMap;
QMap <QString, QString> docMap;
QMap <QString, QString> pluginMap;
QMap <QString, bool> providedMap;

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

QString InfoBackend::constructionStringForKey(QString key) const
{
    if (constructionStringMap.contains(key))
        return constructionStringMap.value(key);
    else
        return QString();
}

QString InfoBackend::typeForKey(QString key) const
{
     if (typeMap.contains(key))
        return typeMap.value(key);
    else
        return QString();
}

QString InfoBackend::docForKey(QString key) const
{
    if (docMap.contains(key))
        return docMap.value(key);
    else
        return QString();
}

QString InfoBackend::pluginForKey(QString key) const
{
    if (pluginMap.contains(key))
        return pluginMap.value(key);
    else
        return QString();
}

bool InfoBackend::keyExists(QString key) const
{
    if (typeMap.contains(key))
        return true;
    else
        return false;
}

bool InfoBackend::keyProvided(QString key) const
{
    if (providedMap.contains(key))
        return providedMap.value(key);
    else
        return false;
}

void InfoBackend::connectNotify(const char *signal)
{
}

void InfoBackend::disconnectNotify(const char *signal)
{
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

void InfoBackend::fireKeyDataChanged(const QString& key)
{
    emit keyDataChanged(key);
}

/* ContextRegistryInfoUnitTest */

class ContextPropertyInfoUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void key();
    void doc();
    void type();
    void exists();
    void provided();
    void providerDBusName();
    void providerDBusType();
    void plugin();
    void constructionString();
    void typeChanged();
    void providerChanged();
    void providedChanged();
    void pluginChanged();
};

void ContextPropertyInfoUnitTest::initTestCase()
{
    constructionStringMap.clear();
    constructionStringMap.insert("Battery.Charging", "system:org.freedesktop.ContextKit.contextd");
    constructionStringMap.insert("Media.NowPlaying", "session:com.nokia.musicplayer");

    typeMap.clear();
    typeMap.insert("Battery.Charging", "TRUTH");
    typeMap.insert("Media.NowPlaying", "STRING");

    docMap.clear();
    docMap.insert("Battery.Charging", "Battery.Charging doc");
    docMap.insert("Media.NowPlaying", "Media.NowPlaying doc");

    pluginMap.clear();
    pluginMap.insert("Battery.Charging", "contextkit-dbus");
    pluginMap.insert("Media.NowPlaying", "contextkit-dbus");

    providedMap.clear();
    providedMap.insert("Battery.Charging", true);
    providedMap.insert("Media.NowPlaying", true);
}

void ContextPropertyInfoUnitTest::key()
{
    ContextPropertyInfo p("Battery.Charging");
    QCOMPARE(p.key(), QString("Battery.Charging"));
}

void ContextPropertyInfoUnitTest::doc()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.doc(), QString("Battery.Charging doc"));
    QCOMPARE(p2.doc(), QString("Media.NowPlaying doc"));
    QCOMPARE(p3.doc(), QString());
}

void ContextPropertyInfoUnitTest::type()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.type(), QString("TRUTH"));
    QCOMPARE(p2.type(), QString("STRING"));
    QCOMPARE(p3.type(), QString());
}

void ContextPropertyInfoUnitTest::exists()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.exists(), true);
    QCOMPARE(p2.exists(), true);
    QCOMPARE(p3.exists(), false);
}

void ContextPropertyInfoUnitTest::provided()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.provided(), true);
    QCOMPARE(p2.provided(), true);
    QCOMPARE(p3.provided(), false);
}

void ContextPropertyInfoUnitTest::providerDBusName()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.providerDBusName(), QString("org.freedesktop.ContextKit.contextd"));
    QCOMPARE(p2.providerDBusName(), QString("com.nokia.musicplayer"));
    QCOMPARE(p3.providerDBusName(), QString());
}

void ContextPropertyInfoUnitTest::providerDBusType()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.providerDBusType(), QDBusConnection::SystemBus);
    QCOMPARE(p2.providerDBusType(), QDBusConnection::SessionBus);
    QCOMPARE(p3.providerDBusType(), QDBusConnection::SessionBus);
}

void ContextPropertyInfoUnitTest::plugin()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.plugin(), QString("contextkit-dbus"));
    QCOMPARE(p2.plugin(), QString("contextkit-dbus"));
    QCOMPARE(p3.plugin(), QString());
}

void ContextPropertyInfoUnitTest::constructionString()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.constructionString(), QString("system:org.freedesktop.ContextKit.contextd"));
    QCOMPARE(p2.constructionString(), QString("session:com.nokia.musicplayer"));
    QCOMPARE(p3.constructionString(), QString());
}

void ContextPropertyInfoUnitTest::typeChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy(&p, SIGNAL(typeChanged(QString)));

    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 0);

    typeMap.insert("Battery.Charging", "INT");
    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("INT"));
}

void ContextPropertyInfoUnitTest::providerChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy(&p, SIGNAL(providerChanged(QString)));

    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 0);

    constructionStringMap.insert("Battery.Charging", "system:org.freedesktop.ContextKit.robot");
    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("org.freedesktop.ContextKit.robot"));
}

void ContextPropertyInfoUnitTest::providedChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy(&p, SIGNAL(providedChanged(bool)));

    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 0);

    providedMap.insert("Battery.Charging", false);
    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
}

void ContextPropertyInfoUnitTest::pluginChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy1(&p, SIGNAL(pluginChanged(QString, QString)));
    QSignalSpy spy2(&p, SIGNAL(providerChanged(QString)));

    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);

    pluginMap.insert("Battery.Charging", "test.so");
    constructionStringMap.insert("Battery.Charging", "secret:something");
    currentBackend->fireKeyDataChanged(QString("Battery.Charging"));

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toString(), QString("test.so"));
    QCOMPARE(args1.at(1).toString(), QString("secret:something"));

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toString(), QString(""));
}

#include "contextpropertyinfounittest.moc"
QTEST_MAIN(ContextPropertyInfoUnitTest);
