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

QString InfoBackend::typeForKey(QString key) const
{
    if (key == "Battery.Charging")
        return "TRUTH";
    else if (key == "Media.NowPlaying")
        return "STRING";
    else
        return "";
}

QString InfoBackend::docForKey(QString key) const
{
    if (key == "Battery.Charging")
        return "Battery.Charging doc";
    else if (key == "Media.NowPlaying")
        return "Media.NowPlaying doc";
    else
        return "";
}

QString InfoBackend::pluginForKey(QString key) const
{
    if (key == "Battery.Charging")
        return "contextkit-dbus";
    else if (key == "Media.NowPlaying")
        return "contextkit-dbus";
    else
        return "";
}

bool InfoBackend::keyExists(QString key) const
{
    if (key == "Battery.Charging")
        return true;
    else if (key == "Media.NowPlaying")
        return true;
    else
        return false;
}

bool InfoBackend::keyProvided(QString key) const
{
    if (key == "Battery.Charging")
        return true;
    else if (key == "Media.NowPlaying")
        return false;
    else
        return false;
}

void InfoBackend::connectNotify(const char *signal)
{
}

void InfoBackend::disconnectNotify(const char *signal)
{
}

/*
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
*/

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
};

void ContextPropertyInfoUnitTest::initTestCase()
{
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

#include "contextpropertyinfounittest.moc"
QTEST_MAIN(ContextPropertyInfoUnitTest);
