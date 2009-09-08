/*
 * Copyright (C) 2009 Nokia Corporation.
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

#include "bluezinterface.h"
#include "logging.h"

#include <QDBusObjectPath>
#include <QDBusInterface>
#include <QDBusError>
#include <QDBusVariant>

const QString BluezInterface::serviceName = "org.bluez";
const QString BluezInterface::managerPath = "/";
const QString BluezInterface::managerInterface = "org.bluez.Manager";
const QString BluezInterface::adapterInterface = "org.bluez.Adapter";
QDBusConnection BluezInterface::busConnection = QDBusConnection::systemBus();

BluezInterface::BluezInterface() : manager(0), adapter(0)
{
    busConnection.connect("org.freedesktop.DBus", "/org/freedesktop/DBus",
                          "org.freedesktop.DBus", "NameOwnerChanged",
                          this, SLOT(onNameOwnerChanged(QString, QString, QString)));
    connectToBluez();
}

void BluezInterface::onNameOwnerChanged(QString name, QString /*oldOwner*/, QString newOwner)
{
    if (name == serviceName && newOwner != "") {
        connectToBluez();
    }
}

void BluezInterface::connectToBluez()
{
    if (adapter) {
        busConnection.disconnect(serviceName,
                                 adapterPath,
                                 adapterInterface,
                                 "PropertyChanged",
                                 this, SLOT(onPropertyChanged(QString, QDBusVariant)));
        delete adapter;
        adapter = 0;
    }
    if (manager) {
        delete manager;
        manager = 0;
    }

    manager = new QDBusInterface(serviceName, managerPath, managerInterface, busConnection, this);
    manager->callWithCallback("DefaultAdapter", QList<QVariant>(), this,
                              SLOT(replyDefaultAdapter(QDBusObjectPath)),
                              SLOT(replyDBusError(QDBusError)));
}

void BluezInterface::replyDBusError(QDBusError err)
{
    contextWarning() << "DBus error occured:" << err.message();
    emit failed();
}

void BluezInterface::replyDefaultAdapter(QDBusObjectPath path)
{
    adapterPath = path.path();
    adapter = new QDBusInterface(serviceName, adapterPath, adapterInterface, busConnection, this);
    busConnection.connect(serviceName,
                          path.path(),
                          adapterInterface,
                          "PropertyChanged",
                          this, SLOT(onPropertyChanged(QString, QDBusVariant)));
    adapter->callWithCallback("GetProperties", QList<QVariant>(), this,
                              SLOT(replyGetProperties(QMap<QString, QVariant>)),
                              SLOT(replyDBusError(QDBusError)));
    emit ready();
}

void BluezInterface::onPropertyChanged(QString key, QDBusVariant value)
{
    emit propertyChanged(key, value.variant());
}

void BluezInterface::replyGetProperties(QMap<QString, QVariant> map)
{
    foreach(QString key, map.keys())
        emit propertyChanged(key, map[key]);
}
