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

QMap <QString, ContextProviderInfo> providerMap;
QMap <QString, QString> typeMap;
QMap <QString, QString> docMap;

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

bool InfoBackend::keyDeclared(QString key) const
{
    if (typeMap.contains(key))
        return true;
    else
        return false;
}

const QList<ContextProviderInfo> InfoBackend::listProviders(QString key)
{
    QList<ContextProviderInfo> lst;
    if (providerMap.contains(key))
        lst << providerMap.value(key, ContextProviderInfo("", ""));

    return lst;
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

void InfoBackend::fireKeyChanged(const QString& key)
{
    emit keyChanged(key);
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
    void declared();
    void provided();
    void providers();
    void resolutionStrategy();
    void providerDBusName();
    void providerDBusType();
    void plugin();
    void constructionString();
    void typeChanged();
    void providerChanged();
    void providedChanged();
    void pluginChanged();
    void dbusTypeChanged();
};

void ContextPropertyInfoUnitTest::initTestCase()
{
    providerMap.clear();

    ContextProviderInfo info1("contextkit-dbus", "system:org.freedesktop.ContextKit.contextd");
    providerMap.insert("Battery.Charging", info1);

    ContextProviderInfo info2("contextkit-dbus", "session:com.nokia.musicplayer");
    providerMap.insert("Media.NowPlaying", info2);

    typeMap.clear();
    typeMap.insert("Battery.Charging", "TRUTH");
    typeMap.insert("Media.NowPlaying", "STRING");

    docMap.clear();
    docMap.insert("Battery.Charging", "Battery.Charging doc");
    docMap.insert("Media.NowPlaying", "Media.NowPlaying doc");
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

void ContextPropertyInfoUnitTest::declared()
{
    ContextPropertyInfo p1("Battery.Charging");
    ContextPropertyInfo p2("Media.NowPlaying");
    ContextPropertyInfo p3("Does.Not.Exist");
    QCOMPARE(p1.declared(), true);
    QCOMPARE(p2.declared(), true);
    QCOMPARE(p3.declared(), false);
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

void ContextPropertyInfoUnitTest::providers()
{
    ContextPropertyInfo p("Battery.Charging");
    QList<ContextProviderInfo> providers = p.providers();
    QCOMPARE(providers.size(), 1);
    QCOMPARE(providers.at(0).plugin, QString("contextkit-dbus"));
    QCOMPARE(providers.at(0).constructionString, QString("system:org.freedesktop.ContextKit.contextd"));
}

void ContextPropertyInfoUnitTest::resolutionStrategy()
{
    ContextPropertyInfo p("Battery.Charging");
    QCOMPARE(p.resolutionStrategy(), ContextPropertyInfo::LastValue);
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

    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    spy.takeFirst();

    typeMap.insert("Battery.Charging", "INT");
    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("INT"));
}

void ContextPropertyInfoUnitTest::providerChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy(&p, SIGNAL(providerChanged(QString)));

    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    spy.takeFirst();

    ContextProviderInfo info("contextkit-dbus", "system:org.freedesktop.ContextKit.robot");
    providerMap.insert("Battery.Charging", info);
    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toString(), QString("org.freedesktop.ContextKit.robot"));
}

void ContextPropertyInfoUnitTest::providedChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy(&p, SIGNAL(providedChanged(bool)));

    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    spy.takeFirst();

    providerMap.remove("Battery.Charging");
    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.takeFirst();
    QCOMPARE(args.at(0).toBool(), false);
}

void ContextPropertyInfoUnitTest::pluginChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy1(&p, SIGNAL(pluginChanged(QString, QString)));
    QSignalSpy spy2(&p, SIGNAL(providerChanged(QString)));

    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy1.count(), 1);
    spy1.takeFirst();
    QCOMPARE(spy2.count(), 1);
    spy2.takeFirst();

    ContextProviderInfo info("test.so", "secret:something");
    providerMap.insert("Battery.Charging", info);
    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toString(), QString("test.so"));
    QCOMPARE(args1.at(1).toString(), QString("secret:something"));

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toString(), QString(""));
}

void ContextPropertyInfoUnitTest::dbusTypeChanged()
{
    ContextPropertyInfo p("Battery.Charging");
    QSignalSpy spy(&p, SIGNAL(providerDBusTypeChanged(QDBusConnection::BusType)));

    currentBackend->fireKeyChanged(QString("Battery.Charging"));
    QCOMPARE(spy.count(), 1);
    spy.takeFirst();

    ContextProviderInfo info("contextkit-dbus", "session:org.freedesktop.ContextKit.contextd");
    providerMap.insert("Battery.Charging", info);
    currentBackend->fireKeyChanged(QString("Battery.Charging"));

    QCOMPARE(spy.count(), 1);
}

#include "contextpropertyinfounittest.moc"
QTEST_MAIN(ContextPropertyInfoUnitTest);
