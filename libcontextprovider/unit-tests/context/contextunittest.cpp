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
#include "manager.h"
#include "manageradaptor.h"

namespace ContextProvider {

Manager *firstManagerCreated = NULL;
QVariant *lastVariantSet = NULL;

void resetVariants()
{

    delete lastVariantSet;
    lastVariantSet = NULL;
}

bool Manager::keyIsValid(const QString &key) const
{
    return true;
}

void Manager::increaseSubscriptionCount(const QString &key)
{
}
 
void Manager::decreaseSubscriptionCount(const QString &key)
{
}

const QVariant Manager::keyValue(const QString &key)
{
    return QVariant();
}

void Manager::setKeyValue(const QString &key, const QVariant &v)
{
    delete lastVariantSet;
    lastVariantSet = new QVariant(v);
}

QStringList Manager::getKeys()
{
    return keys;
}

QVariant Manager::getKeyValue(const QString &key)
{
    if (lastVariantSet)
        return QVariant(*lastVariantSet);
    else
        return QVariant();
}

void Manager::fakeFirst(const QString &key)
{
    emit firstSubscriberAppeared(key);
}

void Manager::fakeLast(const QString &key)
{
    emit lastSubscriberDisappeared(key);
}

Manager::Manager(const QStringList &k) : keys(k)
{
    if (firstManagerCreated == NULL)
        firstManagerCreated = this;
}

ManagerAdaptor::ManagerAdaptor(Manager* manager, QDBusConnection *conn)
    : QDBusAbstractAdaptor(manager)
{
}

class ContextUnitTest : public QObject
{
    Q_OBJECT

    Service service;
    Property battery_voltage;
    Property battery_is_charging;

private slots:
    void initTestCase();
    void getProperty();
    void checkSignals();
    void setGetBool();
    void setGetInt();
    void setGetDouble();
    void setGetString();
    void setGetQVariant();
    void unset();

public:
    ContextUnitTest();
};

ContextUnitTest::ContextUnitTest()
    : service (QDBusConnection::SessionBus, "com.test.provider", this),
      battery_voltage (service, "Battery.Voltage", this),
      battery_is_charging (service, "Battery.IsCharging", this)
{
    service.start();
}

// Before all tests
void ContextUnitTest::initTestCase()
{
}

void ContextUnitTest::getProperty()
{
    QCOMPARE(battery_voltage.getKey(), QString("Battery.Voltage"));
    QCOMPARE(battery_voltage.isValid(), true);
    QCOMPARE(battery_is_charging.isValid(), true);
}
    
void ContextUnitTest::checkSignals()
{
    resetVariants();
   
    QSignalSpy spy1(&battery_voltage, SIGNAL(firstSubscriberAppeared(QString)));
    QSignalSpy spy2(&battery_voltage, SIGNAL(lastSubscriberDisappeared(QString)));

    firstManagerCreated->fakeFirst("Battery.Voltage");

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toString(), QString("Battery.Voltage"));
    
    firstManagerCreated->fakeLast("Battery.Voltage");

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toString(), QString("Battery.Voltage"));
}

void ContextUnitTest::setGetBool()
{
    resetVariants();
    battery_is_charging.set(true);
    QCOMPARE(*lastVariantSet, QVariant(true));
    QCOMPARE(battery_is_charging.get(), QVariant(true));
}

void ContextUnitTest::setGetInt()
{
    resetVariants();
    battery_voltage.set(666);
    QCOMPARE(*lastVariantSet, QVariant(666));
    QCOMPARE(battery_voltage.get(), QVariant(666));
    QVERIFY(battery_voltage.isSet());
}

void ContextUnitTest::setGetDouble()
{
    resetVariants();
    battery_voltage.set(0.456);
    QCOMPARE(*lastVariantSet, QVariant(0.456));
    QCOMPARE(battery_voltage.get(), QVariant(0.456));
    QVERIFY(battery_voltage.isSet());
}

void ContextUnitTest::setGetString()
{
    resetVariants();
    battery_voltage.set(QString("Hello!"));
    QCOMPARE(*lastVariantSet, QVariant(QString("Hello!")));
    QCOMPARE(battery_voltage.get(), QVariant("Hello!"));
    QVERIFY(battery_voltage.isSet());
}

void ContextUnitTest::setGetQVariant()
{
    resetVariants();
    battery_voltage.set(QVariant(123));
    QCOMPARE(*lastVariantSet, QVariant(123));
    QCOMPARE(battery_voltage.get(), QVariant(123));
    QVERIFY(battery_voltage.isSet());
}

void ContextUnitTest::unset()
{
    resetVariants();
    QVERIFY(battery_voltage.isSet() == false);
    battery_voltage.set(QVariant(123));
    QVERIFY(battery_voltage.isSet());
    QCOMPARE(battery_voltage.get(), QVariant(123));
    battery_voltage.unset();
    QCOMPARE(battery_voltage.get(), QVariant());
    QVERIFY(!battery_voltage.isSet());
}

#include "contextunittest.moc"

} // end namespace

QTEST_MAIN(ContextProvider::ContextUnitTest);
