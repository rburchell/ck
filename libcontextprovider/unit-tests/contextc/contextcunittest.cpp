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
#include "service.h"
#include "property.h"
#include "group.h"
#include "contextc.h"

namespace ContextProvider {

QList<Property*> propertyList;
QList<Group*> groupList;
QStringList keysList;

QString lastBusName = NULL;
QDBusConnection::BusType lastConnectionType;
QVariant *lastVariantSet = NULL;
int lastSubscribed = 0;
void *lastUserData = NULL;

/* Mocked implementation of Group */

Group::Group(QObject *parent)
{
    groupList.append(this);
}

void Group::add(const Property &prop)
{
    props.insert (&prop);
    keyList.append (prop.getKey());
}

Group::~Group()
{
    groupList.removeAll(this);
}

QSet<const Property *> Group::getProperties()
{
    return props;
}

void Group::fakeFirst()
{
    Q_EMIT firstSubscriberAppeared();
}

void Group::fakeLast()
{
    Q_EMIT lastSubscriberDisappeared();
}

/* Mocked implementation of Property */

void resetVariants()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

void emitFirstOn(const QString &k)
{
    Q_FOREACH (Property* c, propertyList) {
        if (c->getKey() == k)
            c->fakeFirst();
    }

    Q_FOREACH (Group* cg, groupList) {
        if (cg->keyList.contains(k))
            cg->fakeFirst();
    }
}

void emitLastOn(const QString &k)
{
    Q_FOREACH (Property* c, propertyList) {
        if (c->getKey() == k)
            c->fakeLast();
    }

    Q_FOREACH (Group* cg, groupList) {
        if (cg->keyList.contains(k))
            cg->fakeLast();
    }
}

Service::Service(QDBusConnection::BusType busType, const QString &busName, QObject *parent)
{
    lastBusName = busName;
    lastConnectionType = busType;
    keysList.clear();
}

void Service::start()
{
}

void Service::stop()
{
}

void Service::restart()
{
}


void Service::setValue (const QString &key, const QVariant &val)
{
    delete lastVariantSet;
    lastVariantSet = new QVariant(val);    
}

void Property::setValue(const QVariant &v)
{
    delete lastVariantSet;
    lastVariantSet = new QVariant(v);
}

void Property::unsetValue()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

Property::Property(const QString &name, QObject *obj)
    : QObject(obj), key(name)
{
    delete lastVariantSet;
    lastVariantSet = NULL;
    propertyList.append(this);
    keysList.append(name);
}

Property::Property(Service &, const QString &name, QObject *obj)
    : QObject(obj), key(name)
{
    delete lastVariantSet;
    lastVariantSet = NULL;
    propertyList.append(this);
    keysList.append(name);
}

Property::~Property()
{
    propertyList.removeAll(this);
}

const QString Property::getKey() const
{
    return key;
}

void Property::fakeFirst()
{
    Q_EMIT firstSubscriberAppeared(key);
}

void Property::fakeLast()
{
    Q_EMIT lastSubscriberDisappeared(key);
}

class ContextCUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void startStopStart();
    void installKey();
    void installGroup();
    void setInterger();
    void setBoolean();
    void setString();
    void setDouble();
    void setMap();
    void setList();
    void testMapList1();
    void testMapList2();
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
    QCOMPARE(lastBusName, QString("com.test.provider"));
    QCOMPARE(keysList.size(), 0);
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
    QVERIFY(keysList.contains("Battery.OnBattery"));
    QVERIFY(keysList.contains("Battery.Power"));
    QCOMPARE(keysList.length(), 2);

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
    const char *keys[] = {
        "Location.Lat",
        "Location.Lon",
        NULL
    };
    context_provider_install_group((char **)keys, 0, MagicCallback, this);
    QVERIFY(keysList.contains("Location.Lat"));
    QVERIFY(keysList.contains("Location.Lon"));
    QCOMPARE(keysList.length(), 2);

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

void ContextCUnitTest::setMap()
{
    context_provider_install_key("Something.Big", 0, MagicCallback, this);
    void *map = context_provider_map_new();
    context_provider_map_set_integer(map, "i_am_int", 123123);
    context_provider_map_set_double(map, "i_are_baboon", 8.54);
    context_provider_map_set_boolean(map, "i_am_weasel", false);
    context_provider_map_set_string(map, "i_am_string", "crime and punishment");
    context_provider_set_map("Something.Big", map, TRUE);
    QVariantMap expected;
    expected.insert("i_am_int", QVariant(123123));
    expected.insert("i_are_baboon", QVariant(8.54));
    expected.insert("i_am_weasel", QVariant(false));
    expected.insert("i_am_string", QVariant("crime and punishment"));
    QCOMPARE(*lastVariantSet, QVariant(expected));
}

void ContextCUnitTest::setList()
{
    context_provider_install_key("Something.Long", 0, MagicCallback, this);
    void *list = context_provider_list_new();
    context_provider_list_add_integer(list, 543);
    context_provider_list_add_double(list, 1.4142);
    context_provider_list_add_boolean(list, true);
    context_provider_list_add_string(list, "catcher in the rye");
    context_provider_set_list("Something.Long", list, TRUE);
    QVariantList expected;
    expected << 543 << 1.4142 << true << "catcher in the rye";
    QCOMPARE(*lastVariantSet, QVariant(expected));
}

void ContextCUnitTest::testMapList1()
{
    context_provider_install_key("Anything.You.Desire", 0, MagicCallback, this);
    void *list = context_provider_list_new();
    context_provider_list_add_integer(list, 144);
    void *map = context_provider_map_new();
    context_provider_map_set_string(map, "peach", "puff");
    context_provider_map_set_list(map, "numbers", list);
    context_provider_set_map("Anything.You.Desire", map, TRUE);
    QVariantMap expected;
    expected.insert("peach", "puff");
    expected.insert("numbers", QVariantList() << 144);
    QCOMPARE(*lastVariantSet, QVariant(expected));
}

void ContextCUnitTest::testMapList2()
{
    context_provider_install_key("Copy.Paste", 0, MagicCallback, this);
    void *map = context_provider_map_new();
    context_provider_map_set_string(map, "peach", "puff");
    void *list = context_provider_list_new();
    context_provider_list_add_integer(list, 144);
    context_provider_list_add_map(list, map);
    context_provider_set_list("Copy.Paste", list, TRUE);
    QVariantList expected;
    QVariantMap emap;
    emap.insert("peach", "puff");
    expected << 144 << emap;
    QCOMPARE(*lastVariantSet, QVariant(expected));
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
    const char *keys[] = {
        "Location.Lat",
        "Location.Lon",
        NULL
    };

    context_provider_install_group((char **)keys, 1, MagicCallback, this);

    context_provider_set_integer("Location.Lat", 123);
    context_provider_set_integer("Location.Lat", 456);
    emitFirstOn("Location.Lat");
    QVERIFY(lastVariantSet == NULL);
}

#include "contextcunittest.moc"

} // end namespace

QTEST_MAIN(ContextProvider::ContextCUnitTest);
