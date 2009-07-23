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

#define LOW_BATTERY_THRESHOLD 10

QStringList HalProvider::keys()
{
    QStringList list;
    list << "Battery.OnBattery";
    list << "Battery.ChargePercentage";
    list << "Battery.LowBattery";
    return list;
}

void HalProvider::initialize()
{
    contextDebug() << F_HAL << "Initializing hal provider";
    group = new SignalGrouper(keys(), this);

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
    QVariant chargePercentage = batteryDevice->readValue("battery.charge_level.percentage");
    QVariant isDischarging = batteryDevice->readValue("battery.rechargeable.is_discharging");
    QVariant isCharging = batteryDevice->readValue("battery.rechargeable.is_charging");

    // Calculate and set ChargePercentage
    if (chargePercentage != QVariant()) 
        Context("Battery.ChargePercentage").set(chargePercentage.toInt());
    else
        Context("Battery.ChargePercentage").unset();

    // Calculate and set OnBattery
    if (isDischarging != QVariant()) 
        Context("Battery.OnBattery").set(isDischarging.toBool());
    else
        Context("Battery.OnBattery").unset();

    // Calculate the LowBattery
    if (isDischarging == QVariant()) 
        Context("Battery.LowBattery").unset();
    else if (isDischarging.toBool() == false) 
        Context("Battery.LowBattery").set(false);
    else if (chargePercentage == QVariant())
        Context("Battery.LowBattery").unset();
    else if (chargePercentage.toInt() < LOW_BATTERY_THRESHOLD)
        Context("Battery.LowBattery").set(true);
    else {
        Context("Battery.LowBattery").set(false);
    }
}

