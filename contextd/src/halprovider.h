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

#ifndef HALPROVIDER_H
#define HALPROVIDER_H

#include <QVariant>
#include <QStringList>
#include <QObject>
#include "provider.h"
#include "contextgroup.h"
#include "haldeviceinterface.h"

struct BatteryInfo;

class HalProvider : public Provider
{
    Q_OBJECT

public:
    virtual QStringList keys();
    virtual void initialize();

private:
    void updateProperties();
    
    ContextGroup *group;
    Context *onBattery;
    Context *lowBattery;
    Context *chargePercentage;
    HalDeviceInterface *batteryDevice;


private slots:
    void onFirstSubscriberAppeared();
    void onLastSubscriberDisappeared();
    void onDevicePropertyModified();
};

#endif // HALPROVIDER_H
