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
#include "halprovider.h"
#include "context.h"
#include "halmanagerinterface.h"

QVariant *chargeLevel = NULL;
QVariant *isCharging = NULL;
QVariant *isDischarging = NULL;
ContextGroup *lastContextGroup = NULL;

void clearVariants()
{
    delete chargeLevel; chargeLevel = NULL;
    delete isCharging; isCharging = NULL;
    delete isDischarging; isDischarging = NULL;
}

/* Mocked Context */

QHash<QString, QVariant> values;

Context::Context(const QString &k, QObject *parent) : key(k)
{
}

void Context::set(const QVariant &v)
{
    values.insert(key, v);
}

void Context::unset()
{
    values.insert(key, QVariant());
}

QVariant Context::get()
{
    if (values.contains(key))
        return values.value(key);
    else
        return QVariant();
}

/* Mocked HalDeviceInterface */

HalDeviceInterface::HalDeviceInterface(const QDBusConnection connection, const QString &busName, const QString objectPath, QObject *parent)
{
}

QVariant HalDeviceInterface::readValue(const QString &prop)
{
    if (prop == "battery.charge_level.percentage" && chargeLevel != NULL)
        return QVariant(*chargeLevel);
    else if (prop == "battery.rechargeable.is_discharging" && isDischarging != NULL)
        return QVariant(*isDischarging);
    else if (prop == "battery.rechargeable.is_charging" && isCharging != NULL)
        return QVariant(*isCharging);
    else
        return QVariant();
}

/* Mocked HalManagerInterface */

HalManagerInterface::HalManagerInterface(const QDBusConnection connection, const QString &busName, QObject *parent)
{
}

QStringList HalManagerInterface::findDeviceByCapability(const QString &capability)
{
    QString b("battery1");
    QStringList lst;
    lst.append(b);
    return lst;
}

/* Mocked ContextGroup */

ContextGroup::ContextGroup(QStringList propertiesToWatch, QObject* parent)
{
    lastContextGroup = this;
}

void ContextGroup::fakeFirst()
{
    emit firstSubscriberAppeared();
}

class HalProviderUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void keys();
    void initialValues();
    void checkBasic();
    void verifyOnBattery();

private:
    HalProvider *provider;
};

// Before each test
void HalProviderUnitTest::init()
{
    provider = new HalProvider();
    provider->initialize();
    clearVariants();
}

// After each test
void HalProviderUnitTest::cleanup()
{
    delete provider;
}

void HalProviderUnitTest::keys()
{
    QStringList keys = provider->keys();
    QVERIFY(keys.contains("Battery.OnBattery"));
    QVERIFY(keys.contains("Battery.LowBattery"));
    QVERIFY(keys.contains("Battery.ChargePercentage"));
}

void HalProviderUnitTest::initialValues()
{
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant());
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant());
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant());
}

void HalProviderUnitTest::checkBasic()
{
    isCharging = new QVariant(true);
    isDischarging = new QVariant(false);
    chargeLevel = new QVariant(99);
    lastContextGroup->fakeFirst();

    QVERIFY(Context("Battery.OnBattery").get() != QVariant());
    QVERIFY(Context("Battery.LowBattery").get() != QVariant());
    QVERIFY(Context("Battery.ChargePercentage").get() != QVariant());
}

void HalProviderUnitTest::verifyOnBattery()
{
    // Power on, full charge
    isCharging = new QVariant(true);
    isDischarging = new QVariant(false);
    chargeLevel = new QVariant(100);
    lastContextGroup->fakeFirst();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(true));

    // Power on, half charge
    isCharging = new QVariant(true);
    isDischarging = new QVariant(false);
    chargeLevel = new QVariant(50);
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(true));

    // Power off, half charge
    isCharging = new QVariant(false);
    isDischarging = new QVariant(true);
    chargeLevel = new QVariant(50);
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(false));

    // Power off, full charge
    isCharging = new QVariant(false);
    isDischarging = new QVariant(true);
    chargeLevel = new QVariant(100);
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(false));
}

#include "halproviderunittest.moc"
QTEST_MAIN(HalProviderUnitTest);
