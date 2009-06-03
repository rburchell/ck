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

/*!
  \class ManagerInterface

  \brief Proxy class for using the DBus interface
  org.freedesktop.ContextKit.Manager asynchronously.

  Implements methods for constructing the interface objects (given the DBus
  type (session or system), and bus name) and calling the function GetSubscriber
  asynchronously.
*/

#include "managerinterface.h"
#include "safedbuspendingcallwatcher.h"
#include "sconnect.h"
#include "logging.h"

#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusObjectPath>

const QString ManagerInterface::interfaceName = "org.freedesktop.ContextKit.Manager";
const QString ManagerInterface::objectPath = "/org/freedesktop/ContextKit/Manager";

/// Constructs the ManagerInterface. Connects to the DBus object specified by
/// \a busType (session or system bus) and \a busName.
ManagerInterface::ManagerInterface(const QDBusConnection::BusType busType, const QString &busName, QObject *parent)
    : iface(0), getSubscriberFailed(false)
{
    QDBusConnection connection("");

    // Create DBus connection
    if (busType == QDBusConnection::SessionBus) {
        connection = QDBusConnection::sessionBus();
    } else if (busType == QDBusConnection::SystemBus) {
        connection = QDBusConnection::systemBus();
    } else {
        contextCritical() << "Invalid bus type: " << busType;
        return;
    }

    if (!connection.isConnected()) {
        contextCritical() << "Couldn't connect to DBUS.";
        return;
    }

    // Create the DBus interface
    iface = new QDBusInterface(busName, objectPath, interfaceName, connection, this);
}

/// Calls the GetSubscriber function over DBus asynchronously.
void ManagerInterface::getSubscriber()
{
    if (iface == 0) {
        getSubscriberFailed = true;
        emit getSubscriberFinished("");
    } else {
        getSubscriberFailed = false;

        // Construct the asynchronous call
        // FIXME: this four lines are repeated everywhere, should have a util function for it
        QDBusPendingCall getSubscriberCall = iface->asyncCall("GetSubscriber");
        SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(getSubscriberCall, this);
        sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
                 this, SLOT(onGetSubscriberFinished(QDBusPendingCallWatcher *)));
    }
}

/// Is called when the asynchronous call to GetSubscriber has finished. Emits
/// the signal getSubscriberFinished if the call finished successfully.
void ManagerInterface::onGetSubscriberFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    if (reply.isError()) {
        // Possible causes of the error:
        // The provider is not running
        // The provider didn't implement the needed interface + function
        // The function resulted in an error
        contextWarning() << "Provider error while getting the subscriber object:" << reply.error().message();
        getSubscriberFailed = true;
        emit getSubscriberFinished("");
    } else {
        QDBusObjectPath path = reply.argumentAt<0>();
        emit getSubscriberFinished(path.path());
    }
}

bool ManagerInterface::isGetSubscriberFailed() const
{
    return getSubscriberFailed;
}
