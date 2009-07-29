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

QVariant *halChargePercentage = NULL;
QVariant *halChargeCurrent = NULL;
QVariant *halIsCharging = NULL;
QVariant *halIsDischarging = NULL;
QVariant *halRate = NULL;
QVariant *halLastFull = NULL;

ContextGroup *lastContextGroup = NULL;
HalDeviceInterface *lastDeviceInterface = NULL;
bool hasBattery = true;

QHash<QString, QVariant> values;

void clearVariantsAndValues()
{
    delete halChargePercentage; halChargePercentage = NULL;
    delete halChargeCurrent; halChargeCurrent = NULL;
    delete halIsCharging; halIsCharging = NULL;
    delete halIsDischarging; halIsDischarging = NULL;
    delete halRate; halRate = NULL;
    delete halLastFull; halLastFull = NULL;
    values.clear();
}

/* Mocked Context */

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
    lastDeviceInterface = this;
}

QVariant HalDeviceInterface::readValue(const QString &prop)
{
    if (prop == "battery.charge_level.percentage" && halChargePercentage != NULL)
        return QVariant(*halChargePercentage);
    if (prop == "battery.charge_level.current" && halChargeCurrent != NULL)
        return QVariant(*halChargeCurrent);
    else if (prop == "battery.rechargeable.is_discharging" && halIsDischarging != NULL)
        return QVariant(*halIsDischarging);
    else if (prop == "battery.rechargeable.is_charging" && halIsCharging != NULL)
        return QVariant(*halIsCharging);
    else if (prop == "battery.charge_level.rate" && halRate != NULL)
        return QVariant(*halRate);
    else if (prop == "battery.charge_level.last_full" && halLastFull != NULL)
        return QVariant(*halLastFull);
    else
        return QVariant();
}

void HalDeviceInterface::fakeModified()
{
    emit PropertyModified();
}

/* Mocked HalManagerInterface */

HalManagerInterface::HalManagerInterface(const QDBusConnection connection, const QString &busName, QObject *parent)
{
}

QStringList HalManagerInterface::findDeviceByCapability(const QString &capability)
{
    if (hasBattery) {
        QString b("battery1");
        QStringList lst;
        lst.append(b);
        return lst;
    } else
        return QStringList();
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

void ContextGroup::fakeLast()
{
    emit lastSubscriberDisappeared();
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
    void verifyBaseProperties();
    void verifyTillFullProperties();
    void verifyTillLowProperties();
    void firstLastFirst();
    void noBattery();
    void noHalInfo();

private:
    HalProvider *provider;
};

// Before each test
void HalProviderUnitTest::init()
{
    provider = new HalProvider();
    provider->initialize();
    clearVariantsAndValues();
    hasBattery = true;
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
    halIsCharging = new QVariant(true);
    halIsDischarging = new QVariant(false);
    halChargePercentage = new QVariant(99);
    lastContextGroup->fakeFirst();

    QVERIFY(Context("Battery.OnBattery").get() != QVariant());
    QVERIFY(Context("Battery.LowBattery").get() != QVariant());
    QVERIFY(Context("Battery.ChargePercentage").get() != QVariant());
}

void HalProviderUnitTest::verifyBaseProperties()
{
    // Power on, full charge
    halIsCharging = new QVariant(true);
    halIsDischarging = new QVariant(false);
    halChargePercentage = new QVariant(100);
    lastContextGroup->fakeFirst();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(false));
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant(100));
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant(false));

    // Power on, half charge
    halIsCharging = new QVariant(true);
    halIsDischarging = new QVariant(false);
    halChargePercentage = new QVariant(50);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(false));
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant(50));
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant(false));

    // Power off, half charge
    halIsCharging = new QVariant(false);
    halIsDischarging = new QVariant(true);
    halChargePercentage = new QVariant(50);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(true));
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant(50));
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant(false));

    // Power off, full charge
    halIsCharging = new QVariant(false);
    halIsDischarging = new QVariant(true);
    halChargePercentage = new QVariant(100);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(true));
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant(100));
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant(false));

    // Power off, small charge
    halIsCharging = new QVariant(false);
    halIsDischarging = new QVariant(true);
    halChargePercentage = new QVariant(1);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(true));
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant(1));
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant(true));

    lastContextGroup->fakeLast();
}

void HalProviderUnitTest::verifyTillFullProperties()
{
    // 5h charge hours left
    halIsCharging = new QVariant(true);
    halLastFull = new QVariant(1000);
    halChargeCurrent = new QVariant(500);
    halRate = new QVariant(100);
    lastContextGroup->fakeFirst();
    QCOMPARE(Context("Battery.TimeUntilFull").get(), QVariant(5 * 3600));

    // Almost there...
    halChargeCurrent = new QVariant(999);
    halRate = new QVariant(100);
    lastDeviceInterface->fakeModified();
    QVERIFY(Context("Battery.TimeUntilFull").get().toInt() < 60);

    // Full
    halChargeCurrent = new QVariant(1000);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.TimeUntilFull").get(), QVariant(0));

    // Not even charging
    halChargeCurrent = new QVariant(100);
    halIsCharging = new QVariant(false);
    halIsDischarging = new QVariant(true);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.TimeUntilFull").get(), QVariant());
}

void HalProviderUnitTest::verifyTillLowProperties()
{
    // 4h
    halIsCharging = new QVariant(false);
    halIsDischarging = new QVariant(true);
    halLastFull = new QVariant(1000);
    halChargeCurrent = new QVariant(500);
    halRate = new QVariant(100);
    lastContextGroup->fakeFirst();
    QCOMPARE(Context("Battery.TimeUntilLow").get(), QVariant(4 * 3600));

    // Low already
    halChargeCurrent = new QVariant(1);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.TimeUntilLow").get(), QVariant(0));

    // Charging
    halIsCharging = new QVariant(true);
    halIsDischarging = new QVariant(false);
    lastDeviceInterface->fakeModified();
    QCOMPARE(Context("Battery.TimeUntilLow").get(), QVariant());
}

void HalProviderUnitTest::firstLastFirst()
{
    halIsCharging = new QVariant(true);
    halIsDischarging = new QVariant(false);
    halChargePercentage = new QVariant(100);
    lastContextGroup->fakeFirst();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(false));
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant(100));
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant(false));

    lastContextGroup->fakeLast();

    halIsCharging = new QVariant(false);
    halIsDischarging = new QVariant(true);
    halChargePercentage = new QVariant(4);
    
    lastContextGroup->fakeFirst();
    
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant(true));
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant(4));
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant(true));
}

void HalProviderUnitTest::noBattery()
{
    hasBattery = false;

    QCOMPARE(Context("Battery.OnBattery").get(), QVariant());
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant());
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant());

    lastContextGroup->fakeFirst();

    QCOMPARE(Context("Battery.OnBattery").get(), QVariant());
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant());
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant());
}

void HalProviderUnitTest::noHalInfo()
{
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant());
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant());
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant());
    lastContextGroup->fakeFirst();
    QCOMPARE(Context("Battery.OnBattery").get(), QVariant());
    QCOMPARE(Context("Battery.LowBattery").get(), QVariant());
    QCOMPARE(Context("Battery.ChargePercentage").get(), QVariant());
}

#include "halproviderunittest.moc"
QTEST_MAIN(HalProviderUnitTest);
