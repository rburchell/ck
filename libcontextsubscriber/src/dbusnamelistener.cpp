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

#include "dbusnamelistener.h"
#include "safedbuspendingcallwatcher.h"
#include "sconnect.h"

#include <QDBusConnectionInterface>
#include <QPair>
#include <QMap>
#include <QString>

QMap<QPair<QDBusConnection::BusType, QString>, DBusNameListener*> DBusNameListener::listenerInstances;

DBusNameListener* DBusNameListener::instance(const QDBusConnection::BusType busType, const QString &busName)
{
    QPair<QDBusConnection::BusType, QString> lookupValue(busType, busName);
    if (!listenerInstances.contains(lookupValue))
            listenerInstances.insert(lookupValue,
                                     new DBusNameListener(busType, busName));

    return listenerInstances[lookupValue];
}

DBusNameListener::DBusNameListener(const QDBusConnection::BusType busType, const QString &busName) :
    busName(busName), servicePresent(false)
{
    QDBusConnection connection("");

    // Create DBus connection
    if (busType == QDBusConnection::SessionBus) {
        connection = QDBusConnection::sessionBus();
    } else if (busType == QDBusConnection::SystemBus) {
        connection = QDBusConnection::systemBus();
    } else {
        qCritical() << "Invalid bus type: " << busType;
        return;
    }

    if (!connection.isConnected()) {
        qCritical() << "Couldn't connect to DBUS: ";
        return;
    }

    sconnect(connection.interface(),
             SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
             this,
             SLOT(onServiceOwnerChanged(const QString&, const QString&, const QString&)));

    // Check if the service is already there
    QDBusPendingCall nameHasOwnerCall = connection.interface()->asyncCall("NameHasOwner", busName);
    SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(nameHasOwnerCall, this);
    sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(onNameHasOwnerFinished(QDBusPendingCallWatcher *)));
}

void DBusNameListener::onServiceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    if (name == busName) {
        if (oldOwner == "") {
            setServicePresent();
        } else if (newOwner == "") {
            setServiceGone();
        }
    }
}

void DBusNameListener::setServicePresent()
{
    if (servicePresent == false) {
        servicePresent = true;
        emit nameAppeared();
    }
}

void DBusNameListener::setServiceGone()
{
    if (servicePresent == true) {
        servicePresent = false;
        emit nameDisappeared();
    }
}

void DBusNameListener::onNameHasOwnerFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<bool> reply = *watcher;
    if (reply.isError() == false && reply.argumentAt<0>() == true) {
        // The name has an owner
        setServicePresent();
    }
}

bool DBusNameListener::isServicePresent() const
{
    return servicePresent;
}
