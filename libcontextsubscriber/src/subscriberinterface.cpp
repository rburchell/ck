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

/// Constructs the subscriber interface. Connects to the Subscriber object
/// specified by the bus type (session or system bus), the bus name and object path.
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
    sconnect(iface, SIGNAL(Changed(const QMap<QString, QVariant>&, const QStringList &)),
             this, SLOT(onChanged(const QMap<QString, QVariant>&, const QStringList &)));
}

/// Calls the Subscribe function over DBus asynchronously.
void SubscriberInterface::subscribe(QStringList keys)
{
    if (iface == 0 || keys.size() == 0) {
        return;
    }
    // Construct the asynchronous call
    QDBusPendingCall subscribeCall = iface->asyncCall("Subscribe", keys);
    SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(subscribeCall, this);

    sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(onSubscribeFinished(QDBusPendingCallWatcher *)));
}

/// Calls the Unsubscribe function over DBus asynchronously.
void SubscriberInterface::unsubscribe(QStringList keys)
{
    if (iface == 0 || keys.size() == 0) {
        return;
    }
    // Construct the asynchronous call
    iface->asyncCall("Unsubscribe", keys);
    // we are just not interested in the possible errors, because we can't do anything about them
}

/// Processes the results of the Changed signal which comes over DBus.
void SubscriberInterface::onChanged(const QMap<QString, QVariant> &values, const QStringList& unknownKeys)
{
//    qDebug() << "SubscriberInterface::onChanged";
    QMap<QString, QVariant> copy = values;
    emit valuesChanged(mergeNullsWithMap(copy, unknownKeys), false);
}

/// A helper function. Sets the values of given keys to a null QVariant in a QMap.
QMap<QString, QVariant>& SubscriberInterface::mergeNullsWithMap(QMap<QString, QVariant> &map, QStringList nulls) const
{
    foreach (QString null, nulls) {
        if (map.contains(null))
            qWarning() << "PROVIDER ERROR: provided unknown and a value for " << null;
        else
            map[null] = QVariant();
    }
    return map;
}

/// Is called when the asynchronous DBus call to Subscribe has finished.
void SubscriberInterface::onSubscribeFinished(QDBusPendingCallWatcher* watcher)
{
    qDebug() << "onSubscribeFinished";
    QDBusPendingReply<QMap<QString, QVariant>, QStringList> reply = *watcher;
    if (reply.isError()) {
        // Possible causes of the error:
        // The provider is not running
        // The provider didn't implement the needed interface + function
        // The function resulted in an error
        qWarning() << "Provider error while subscribing:" << reply.error().message();
    } else {
        QMap<QString, QVariant> subscribeTimeValues = reply.argumentAt<0>();
        QStringList unknowns = reply.argumentAt<1>();

        // FIXME: the protocol should be better, this is just a workaround
        emit valuesChanged(mergeNullsWithMap(subscribeTimeValues, unknowns), true);
    }
    emit subscribeFinished(subscribeTimeValues.keys());
}

SubscriberSignallingInterface::SubscriberSignallingInterface(const QString &dbusName, const QString& objectPath, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(dbusName, objectPath, interfaceName.toStdString().c_str(), connection, parent)
{
}

SubscriberSignallingInterface::~SubscriberSignallingInterface()
{
}

