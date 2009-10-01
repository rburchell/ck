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

namespace ContextProvider {

Manager *manager;
ServiceBackend *serviceBackend;
QVariant *lastVariantSet = NULL;

void resetVariants()
{
    delete lastVariantSet;
    lastVariantSet = NULL;
}

bool Manager::keyIsValid(const QString &key) const
{
    return (keys.contains(key));
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

Manager::Manager()
{
}

void Manager::addKey(const QString &key)
{
    keys << key;
}

// Mock implementation of Service

Service::Service()
{
    backend = new ContextProvider::ServiceBackend();
}

// Mock implementation of ServiceBackend
ServiceBackend* ServiceBackend::defaultServiceBackend = NULL;

Manager *ServiceBackend::manager()
{
    return ContextProvider::manager;
}

class PropertyUnitTest : public QObject
{
    Q_OBJECT

    Service service;
    Property* battery_voltage;
    Property* battery_is_charging;

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void getProperty();
    void checkSignals();
    void setGetBool();
    void setGetInt();
    void setGetDouble();
    void setGetString();
    void setGetQVariant();
    void unset();
};

// Before all tests
void PropertyUnitTest::initTestCase()
{
}

void PropertyUnitTest::init()
{
    manager = new Manager();
    battery_voltage = new Property(service, "Battery.Voltage");
    battery_is_charging = new Property(service, "Battery.IsCharging");
}

void PropertyUnitTest::cleanup()
{
    delete battery_voltage; battery_voltage = NULL;
    delete battery_is_charging; battery_is_charging = NULL;
    delete manager; manager = NULL;
}

void PropertyUnitTest::getProperty()
{
    QCOMPARE(battery_voltage->key(), QString("Battery.Voltage"));
}

void PropertyUnitTest::checkSignals()
{
    resetVariants();

    QSignalSpy spy1(battery_voltage, SIGNAL(firstSubscriberAppeared(QString)));
    QSignalSpy spy2(battery_voltage, SIGNAL(lastSubscriberDisappeared(QString)));

    manager->fakeFirst("Battery.Voltage");

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toString(), QString("Battery.Voltage"));

    manager->fakeLast("Battery.Voltage");

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toString(), QString("Battery.Voltage"));
}

void PropertyUnitTest::setGetBool()
{
    resetVariants();
    battery_is_charging->setValue(true);
    QCOMPARE(*lastVariantSet, QVariant(true));
    QCOMPARE(battery_is_charging->value(), QVariant(true));
}

void PropertyUnitTest::setGetInt()
{
    resetVariants();
    battery_voltage->setValue(666);
    QCOMPARE(*lastVariantSet, QVariant(666));
    QCOMPARE(battery_voltage->value(), QVariant(666));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::setGetDouble()
{
    resetVariants();
    battery_voltage->setValue(0.456);
    QCOMPARE(*lastVariantSet, QVariant(0.456));
    QCOMPARE(battery_voltage->value(), QVariant(0.456));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::setGetString()
{
    resetVariants();
    battery_voltage->setValue(QString("Hello!"));
    QCOMPARE(*lastVariantSet, QVariant(QString("Hello!")));
    QCOMPARE(battery_voltage->value(), QVariant("Hello!"));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::setGetQVariant()
{
    resetVariants();
    battery_voltage->setValue(QVariant(123));
    QCOMPARE(*lastVariantSet, QVariant(123));
    QCOMPARE(battery_voltage->value(), QVariant(123));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::unset()
{
    resetVariants();
    QVERIFY(battery_voltage->isSet() == false);
    battery_voltage->setValue(QVariant(123));
    QVERIFY(battery_voltage->isSet());
    QCOMPARE(battery_voltage->value(), QVariant(123));
    battery_voltage->unsetValue();
    QCOMPARE(battery_voltage->value(), QVariant());
    QVERIFY(!battery_voltage->isSet());
}

#include "propertyunittest.moc"

} // end namespace

QTEST_MAIN(ContextProvider::PropertyUnitTest);
