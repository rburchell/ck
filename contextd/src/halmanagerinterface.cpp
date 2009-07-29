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

#include "halmanagerinterface.h"
//#include "sconnect.h"
#include "logging.h"
#include <QDBusInterface>

#define INTERFACE_NAME  "org.freedesktop.Hal.Manager"
#define OBJECT_PATH     "/org/freedesktop/Hal/Manager"

HalManagerInterface::HalManagerInterface(const QDBusConnection connection, const QString &busName, QObject *parent)
    : QDBusAbstractInterface(busName, OBJECT_PATH, INTERFACE_NAME, connection, parent)
{
}

QStringList HalManagerInterface::findDeviceByCapability(const QString &capability)
{
    QDBusMessage msg = call(QDBus::Block, "FindDeviceByCapability", capability);
    return msg.arguments().at(0).toStringList();
}

