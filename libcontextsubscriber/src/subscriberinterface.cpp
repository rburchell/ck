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
    sconnect(iface, SIGNAL(Changed(const QMap<QString, QVariant>&, const QStringList &)),
             this, SLOT(onChanged(const QMap<QString, QVariant>&, const QStringList &)));
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

void SubscriberInterface::onChanged(const QMap<QString, QVariant> &values, const QStringList& unknownKeys)
{
    qDebug() << "SubscriberInterface::onChanged";
    qDebug() << values;

}

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
        foreach (QString unknown, unknowns) {
            if (subscribeTimeValues.contains(unknown))
                qWarning() << "PROVIDER ERROR: returned unknown and a value for " << unknown;
            else
                subscribeTimeValues[unknown] = QVariant();
        }

        emit subscribeFinished(subscribeTimeValues);
     }
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

