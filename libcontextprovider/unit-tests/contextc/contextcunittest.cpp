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
#include <stdlib.h>
#include "context.h"
#include "contextc.h"
#include "contextgroup.h"

namespace ContextProvider {

QList<Property*> propertyList;
QList<Group*> groupList;

QString *lastBusName = NULL;
QStringList *lastKeysList = NULL;
QDBusConnection::BusType lastConnectionType;
QVariant *lastVariantSet = NULL;
int lastSubscribed = 0;
void *lastUserData = NULL;

/* Mocked implementation of Group */

Group::Group(QStringList propertiesToWatch, QObject *parent) : keyList(propertiesToWatch)
{
    groupList.append(this);
}

Group::~Group()
{
    groupList.removeAll(this);
}

void Group::fakeFirst()
{
    emit firstSubscriberAppeared();
}

void Group::fakeLast()
{
    emit lastSubscriberDisappeared();
}

/* Mocked implementation of Property */

void resetVariants()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

void emitFirstOn(const QString &k)
{
    foreach (Property* c, propertyList) {
        if (c->getKey() == k)
            c->fakeFirst();
    }

    foreach (Group* cg, groupList) {
        if (cg->keyList.contains(k))
            cg->fakeFirst();
    }
}

void emitLastOn(const QString &k)
{
    foreach (Property* c, propertyList) {
        if (c->getKey() == k)
            c->fakeLast();
    }

    foreach (Group* cg, groupList) {
        if (cg->keyList.contains(k))
            cg->fakeLast();
    }
}

bool Property::initService(QDBusConnection::BusType busType, const QString &busName, const QStringList &keys)
{
    if (lastBusName && lastBusName == busName)
        return false;

    delete lastBusName;
    lastBusName = new QString(busName);

    delete lastKeysList;
    lastKeysList = new QStringList(keys);

    lastConnectionType = busType;
    return true;
}

void Property::stopService(const QString &name)
{
    if (lastBusName && name == lastBusName) {
        delete lastBusName;
        lastBusName = NULL;
        delete lastKeysList;
        lastKeysList = NULL;
    }
}

void Property::set(const QVariant &v)
{
    delete lastVariantSet;
    lastVariantSet = new QVariant(v);
}

void Property::unset()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

Property::Property(const QString &name, QObject *obj) : QObject(obj), key(name)
{
    delete lastVariantSet;
    lastVariantSet = NULL;
    propertyList.append(this);
}

Property::~Property()
{
    propertyList.removeAll(this);
}

QString Property::getKey()
{
    return key;
}

void Property::fakeFirst()
{
    emit firstSubscriberAppeared(key);
}

void Property::fakeLast()
{
    emit lastSubscriberDisappeared(key);
}

class ContextCUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void startStopStart();
    void installKey();
    void installGroup();
    void setInterger();
    void setBoolean();
    void setString();
    void setDouble();
    void clearKeyOnSubscribeKey();
    void clearKeyOnSubscribeGroup();
};

void MagicCallback(int subscribed, void *user_data)
{
    lastSubscribed = subscribed;
    lastUserData = user_data;
}

// Before each test
void ContextCUnitTest::init()
{
    context_provider_stop();
    int res = context_provider_init(DBUS_BUS_SESSION, "com.test.provider");
    QCOMPARE(res, 1);
    QCOMPARE(*lastBusName, QString("com.test.provider"));
    QCOMPARE(lastKeysList->size(), 0);
    QCOMPARE(lastConnectionType, QDBusConnection::SessionBus);
}

void ContextCUnitTest::startStopStart()
{
    context_provider_stop();
    QCOMPARE(context_provider_init(DBUS_BUS_SESSION, "com.test.provider"), 1);
    QCOMPARE(context_provider_init(DBUS_BUS_SESSION, "com.test.provider"), 0);
    context_provider_stop();
    QCOMPARE(context_provider_init(DBUS_BUS_SESSION, "com.test.provider"), 1);
}

void ContextCUnitTest::installKey()
{
    context_provider_install_key("Battery.OnBattery", 0, MagicCallback, this);
    context_provider_install_key("Battery.Power", 0, MagicCallback, this);
    QVERIFY(lastKeysList->contains("Battery.OnBattery"));
    QVERIFY(lastKeysList->contains("Battery.Power"));
    QCOMPARE(lastKeysList->length(), 2);

    emitFirstOn("Battery.OnBattery");
    QCOMPARE(lastSubscribed, 1);
    QCOMPARE(lastUserData, this);

    emitLastOn("Battery.Power");
    QCOMPARE(lastSubscribed, 0);
    QCOMPARE(lastUserData, this);

    emitFirstOn("Battery.Something");
    QCOMPARE(lastSubscribed, 0);
    QCOMPARE(lastUserData, this);
}

void ContextCUnitTest::installGroup()
{
    const char *keys[3];
    keys[0] = "Location.Lat";
    keys[1] = "Location.Lon";
    keys[2] = NULL;

    context_provider_install_group(keys, 0, MagicCallback, this);
    QVERIFY(lastKeysList->contains("Location.Lat"));
    QVERIFY(lastKeysList->contains("Location.Lon"));
    QCOMPARE(lastKeysList->length(), 2);

    emitFirstOn("Location.Lat");
    QCOMPARE(lastSubscribed, 1);
    QCOMPARE(lastUserData, this);

    emitLastOn("Location.Lat");
    QCOMPARE(lastSubscribed, 0);
    QCOMPARE(lastUserData, this);
}

void ContextCUnitTest::setInterger()
{
    context_provider_install_key("Battery.OnBattery", 0, MagicCallback, this);
    context_provider_set_integer("Battery.OnBattery", 666);
    QCOMPARE(*lastVariantSet, QVariant(666));
}

void ContextCUnitTest::setBoolean()
{
    context_provider_install_key("Battery.OnBattery", 0, MagicCallback, this);
    context_provider_set_boolean("Battery.OnBattery", 1);
    QCOMPARE(*lastVariantSet, QVariant(1));
}

void ContextCUnitTest::setString()
{
    context_provider_install_key("Battery.OnBattery", 0, MagicCallback, this);
    context_provider_set_string("Battery.OnBattery", "testing");
    QCOMPARE(*lastVariantSet, QVariant("testing"));
}

void ContextCUnitTest::setDouble()
{
    context_provider_install_key("Battery.OnBattery", 0, MagicCallback, this);
    context_provider_set_double("Battery.OnBattery", 1.23);
    QCOMPARE(*lastVariantSet, QVariant(1.23));
}

void ContextCUnitTest::clearKeyOnSubscribeKey()
{
    context_provider_install_key("Battery.OnBattery", 1, MagicCallback, this);
    context_provider_set_integer("Battery.OnBattery", 666);
    emitFirstOn("Battery.OnBattery");
    QVERIFY(lastVariantSet == NULL);
}

void ContextCUnitTest::clearKeyOnSubscribeGroup()
{
    const char *keys[3];
    keys[0] = "Location.Lat";
    keys[1] = "Location.Lon";
    keys[2] = NULL;

    context_provider_install_group(keys, 1, MagicCallback, this);

    context_provider_set_integer("Location.Lat", 123);
    context_provider_set_integer("Location.Lat", 456);
    emitFirstOn("Location.Lat");
    QVERIFY(lastVariantSet == NULL);
}

#include "contextcunittest.moc"

} // end namespace

QTEST_MAIN(ContextProvider::ContextCUnitTest);
