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

#include "subscriberinterface.h"
#include "safedbuspendingcallwatcher.h"
#include "sconnect.h"

#include <QDebug>

const QString SubscriberSignallingInterface::interfaceName = "org.freedesktop.ContextKit.Subscriber";

SubscriberInterface::SubscriberInterface(
    const QDBusConnection::BusType busType, const QString &busName, const QString& objectPath, QObject *parent)
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
    iface = new SubscriberSignallingInterface(busName, objectPath, connection, this);
    sconnect(iface, SIGNAL(Changed(DBusVariantMap, const QStringList &)),
             this, SLOT(onChanged(DBusVariantMap, const QStringList &)));
}

void SubscriberInterface::subscribe(QStringList keys)
{
    if (iface == 0) {
        return;
    }
    // Construct the asynchronous call
    QDBusPendingCall subscribeCall = iface->asyncCall("Subscribe", keys);
    SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(subscribeCall, this);

    sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(onSubscribeFinished(QDBusPendingCallWatcher *)));
}

void SubscriberInterface::unsubscribe(QStringList keys)
{
}

void SubscriberInterface::onChanged(DBusVariantMap values, const QStringList& unknownKeys)
{
    qDebug() << "SubscriberInterface::onChanged";
}

void SubscriberInterface::onSubscribeFinished(QDBusPendingCallWatcher* watcher)
{
    qDebug() << "onSubscribeFinished";
}

void SubscriberInterface::onUnsubscribeFinished(QDBusPendingCallWatcher* watcher)
{
}

SubscriberSignallingInterface::SubscriberSignallingInterface(const QString &dbusName, const QString& objectPath, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(dbusName, objectPath, interfaceName.toStdString().c_str(), connection, parent)
{
}

SubscriberSignallingInterface::~SubscriberSignallingInterface()
{
}

