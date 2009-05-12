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

#include "managerinterface.h"
#include "safedbuspendingcallwatcher.h"
#include "sconnect.h"

const QString ManagerInterface::interfaceName = "org.freedesktop.ContextKit.Manager";
const QString ManagerInterface::objectPath = "/org/freedesktop/ContextKit/Manager";


ManagerInterface::ManagerInterface(const QDBusConnection::BusType busType, const QString &busName, QObject *parent)
    : iface(0)
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
    iface = new QDBusInterface(busName, objectPath, interfaceName, connection, this);
}

void ManagerInterface::getSubscriber()
{
    if (iface == 0) {
        return;
    }

    QDBusPendingCall getSubscriberCall = iface->asyncCall("GetSubscriber");
    SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(getSubscriberCall, this);

    sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(onGetSubscriberFinished(QDBusPendingCallWatcher *)));
}

void ManagerInterface::onGetSubscriberFinished(QDBusPendingCallWatcher* watcher)
{
     QDBusPendingReply<QDBusObjectPath> reply = *watcher;
     if (reply.isError()) {
         // Possible causes of the error:
         // The provider is not running
         // The provider didn't implement the needed interface + function
         // The function resulted in an error
         qWarning() << "Provider error:" << reply.error().message();
     } else {
         QDBusObjectPath path = reply.argumentAt<0>();
         qDebug() << path.path();

         emit getSubscriberFinished(path.path());
     }
}

