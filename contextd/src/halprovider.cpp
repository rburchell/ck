/*
 * Copyright (C) 2008 Nokia Corporation.
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

#include <QDBusConnection>
#include "halprovider.h"
#include "halmanagerinterface.h"
#include "logging.h"
#include "loggingfeatures.h"
#include "context.h"
#include "sconnect.h"

#define BATTERY_LOW_THRESHOLD       10
#define BATTERY_CHARGE_PERCENTAGE   "battery.charge_level.percentage"
#define BATTERY_CHARGE_CURRENT      "battery.charge_level.current" 
#define BATTERY_CHARGE_RATE         "battery.charge_level.rate"
#define BATTERY_IS_DISCHARGING      "battery.rechargeable.is_discharging"
#define BATTERY_IS_CHARGING         "battery.rechargeable.is_charging"
#define BATTERY_LAST_FULL           "battery.charge_level.last_full"

QStringList HalProvider::keys()
{
    QStringList list;
    list << "Battery.OnBattery";
    list << "Battery.ChargePercentage";
    list << "Battery.LowBattery";
    list << "Battery.TimeUntilLow";
    list << "Battery.TimeUntilFull";
    return list;
}

void HalProvider::initialize()
{
    contextDebug() << F_HAL << "Initializing hal provider";
    group = new ContextGroup(keys(), this);
    onBattery = new Context("Battery.OnBattery", this);
    lowBattery = new Context("Battery.LowBattery", this);
    chargePercentage = new Context("Battery.ChargePercentage", this);
    timeUntilLow = new Context("Battery.TimeUntilLow", this);
    timeUntilFull = new Context("Battery.TimeUntilFull", this);

    sconnect(group, SIGNAL(firstSubscriberAppeared()),
            this, SLOT(onFirstSubscriberAppeared()));

    sconnect(group, SIGNAL(lastSubscriberDisappeared()),
            this, SLOT(onLastSubscriberDisappeared()));
}

void HalProvider::onFirstSubscriberAppeared()
{
    contextDebug() << F_HAL << "First subscriber appeared, connecting to HAL";
    HalManagerInterface manager(QDBusConnection::systemBus(), "org.freedesktop.Hal", this);
    QStringList batteries = manager.findDeviceByCapability("battery");

    if (batteries.length() > 0) {
        contextDebug() << F_HAL << "Analyzing info from battery:" << batteries.at(0);
        batteryDevice = new HalDeviceInterface(QDBusConnection::systemBus(), "org.freedesktop.Hal", 
                                               batteries.at(0), this);

        sconnect(batteryDevice, SIGNAL(PropertyModified()),
                 this, SLOT(onDevicePropertyModified()));

        updateProperties();

    } else {
        contextWarning() << F_HAL << "No valid battery device found";
    }
}


void HalProvider::onLastSubscriberDisappeared()
{
    contextDebug() << F_HAL << "Last subscriber gone, destroying HAL connections";
    delete batteryDevice;
    batteryDevice = NULL;
}

void HalProvider::onDevicePropertyModified()
{
    contextDebug() << F_HAL << "Battery property changed.";
    updateProperties();
}

void HalProvider::updateProperties()
{
    contextDebug() << F_HAL << "Updating properties";

    QVariant chargePercentageV = batteryDevice->readValue(BATTERY_CHARGE_PERCENTAGE);
    QVariant chargeCurrentV = batteryDevice->readValue(BATTERY_CHARGE_CURRENT);
    QVariant isDischargingV = batteryDevice->readValue(BATTERY_IS_DISCHARGING);
    QVariant isChargingV = batteryDevice->readValue(BATTERY_IS_CHARGING);
    QVariant lastFullV = batteryDevice->readValue(BATTERY_LAST_FULL);
    QVariant rateV = batteryDevice->readValue(BATTERY_CHARGE_RATE);
   
    // Calculate and set ChargePercentage
    if (chargePercentageV != QVariant()) 
        chargePercentage->set(chargePercentageV.toInt());
    else
        chargePercentage->unset();

    // Calculate and set OnBattery
    if (isDischargingV != QVariant()) 
        onBattery->set(isDischargingV.toBool());
    else
        onBattery->unset();

    // Calculate the LowBattery
    if (isDischargingV == QVariant()) 
        lowBattery->unset();
    else if (isDischargingV.toBool() == false) 
        lowBattery->set(false);
    else if (chargePercentageV == QVariant())
        lowBattery->unset();
    else if (chargePercentageV.toInt() < BATTERY_LOW_THRESHOLD)
        lowBattery->set(true);
    else {
        lowBattery->set(false);
    }

    // Calculate the time until low. 
    if (chargeCurrentV != QVariant() && 
        isDischargingV != QVariant() && isDischargingV.toBool() == true &&
        lastFullV != QVariant() && lastFullV.toInt() != 0 &&
        rateV != QVariant() && rateV.toInt() != 0) {

        double timeUntilLowV = (chargeCurrentV.toDouble() - BATTERY_LOW_THRESHOLD / 100.0 * lastFullV.toDouble()) / 
                               rateV.toDouble();
        if (timeUntilLowV < 0)
            timeUntilLowV = 0;

        timeUntilLowV *= 3600; // Seconds
        timeUntilLow->set((int) timeUntilLowV);
    } else
        timeUntilLow->unset();

    // Calculate the time until full.
    if (chargeCurrentV != QVariant() &&
        isChargingV != QVariant() && isChargingV.toBool() == true &&
        lastFullV != QVariant() && lastFullV.toInt() != 0 &&
        rateV != QVariant() && rateV.toInt() != 0) {

        double timeUntilFullV = (lastFullV.toDouble() - chargeCurrentV.toDouble()) / rateV.toDouble();
        timeUntilFullV *= 3600; // Seconds
        timeUntilFull->set((int) timeUntilFullV);
    } else
        timeUntilFull->unset();
}

