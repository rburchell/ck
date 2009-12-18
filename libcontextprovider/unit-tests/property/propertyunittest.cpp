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

#include "service.h" // mock
#include "servicebackend.h" // mock

#include "property.h" // to be tested
#include "propertyprivate.h" // to be tested

#include <QtTest/QtTest>
#include <QtCore>
#include <stdlib.h>

namespace ContextProvider {

ServiceBackend *serviceBackend;

// Mock implementation of Service

Service::Service()
{
    backend = new ContextProvider::ServiceBackend();
}

// Mock implementation of ServiceBackend
ServiceBackend* ServiceBackend::defaultServiceBackend = NULL;

void ServiceBackend::addProperty(const QString& key, PropertyPrivate* prop)
{
}

class PropertyUnitTest : public QObject
{
    Q_OBJECT

    Service service;
    Property* battery_voltage;
    Property* battery_is_charging;

private Q_SLOTS:
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
    battery_voltage = new Property(service, "Battery.Voltage");
    battery_is_charging = new Property(service, "Battery.IsCharging");
}

void PropertyUnitTest::cleanup()
{
    delete battery_voltage; battery_voltage = NULL;
    delete battery_is_charging; battery_is_charging = NULL;
}

void PropertyUnitTest::getProperty()
{
    QCOMPARE(battery_voltage->key(), QString("Battery.Voltage"));
}

void PropertyUnitTest::checkSignals()
{
    QSignalSpy spy1(battery_voltage, SIGNAL(firstSubscriberAppeared(QString)));
    QSignalSpy spy2(battery_voltage, SIGNAL(lastSubscriberDisappeared(QString)));

    QVERIFY(battery_voltage->priv);
    Q_EMIT battery_voltage->priv->firstSubscriberAppeared("Battery.Voltage");

    QCOMPARE(spy1.count(), 1);
    QList<QVariant> args1 = spy1.takeFirst();
    QCOMPARE(args1.at(0).toString(), QString("Battery.Voltage"));

    Q_EMIT battery_voltage->priv->lastSubscriberDisappeared("Battery.Voltage");

    QCOMPARE(spy2.count(), 1);
    QList<QVariant> args2 = spy2.takeFirst();
    QCOMPARE(args2.at(0).toString(), QString("Battery.Voltage"));
}

void PropertyUnitTest::setGetBool()
{
    battery_is_charging->setValue(true);
    QCOMPARE(battery_is_charging->value(), QVariant(true));
    QCOMPARE(battery_is_charging->priv->value, QVariant(true));
}

void PropertyUnitTest::setGetInt()
{
    battery_voltage->setValue(666);
    QCOMPARE(battery_voltage->value(), QVariant(666));
    QCOMPARE(battery_voltage->priv->value, QVariant(666));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::setGetDouble()
{
    battery_voltage->setValue(0.456);
    QCOMPARE(battery_voltage->value(), QVariant(0.456));
    QCOMPARE(battery_voltage->priv->value, QVariant(0.456));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::setGetString()
{
    battery_voltage->setValue(QString("Hello!"));
    QCOMPARE(battery_voltage->value(), QVariant("Hello!"));
    QCOMPARE(battery_voltage->priv->value, QVariant("Hello!"));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::setGetQVariant()
{
    battery_voltage->setValue(QVariant(123));
    QCOMPARE(battery_voltage->value(), QVariant(123));
    QCOMPARE(battery_voltage->priv->value, QVariant(123));
    QVERIFY(battery_voltage->isSet());
}

void PropertyUnitTest::unset()
{
    battery_voltage->setValue(QVariant(123));
    QVERIFY(battery_voltage->isSet());
    QCOMPARE(battery_voltage->value(), QVariant(123));
    QCOMPARE(battery_voltage->priv->value, QVariant(123));
    battery_voltage->unsetValue();
    QCOMPARE(battery_voltage->value(), QVariant());
    QCOMPARE(battery_voltage->priv->value, QVariant());
    QVERIFY(!battery_voltage->isSet());
}

#include "propertyunittest.moc"

} // end namespace

QTEST_MAIN(ContextProvider::PropertyUnitTest);
