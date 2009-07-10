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

  \brief Proxy for calling the GetSubscriber asynchronously on ManagerInterface.
*/

#include "managerinterface.h"
#include "safedbuspendingcallwatcher.h"
#include "sconnect.h"
#include "logging.h"

#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusObjectPath>

namespace ContextSubscriber {

const QString ManagerInterface::objectPath = "/org/freedesktop/ContextKit/Manager";
const char *ManagerInterface::interfaceName = "org.freedesktop.ContextKit.Manager";

/// Constructs the ManagerInterface. Connects to the DBus object specified by
/// \a connection and \a busName.
ManagerInterface::ManagerInterface(const QDBusConnection connection, const QString &busName, QObject *parent)
    : QDBusAbstractInterface(busName, objectPath, interfaceName, connection, parent), getSubscriberFailed(false)
{
}

/// Calls the GetSubscriber function over DBus asynchronously.
void ManagerInterface::getSubscriber()
{
    getSubscriberFailed = false;

    // Construct the asynchronous call
    // FIXME: this four lines are repeated everywhere, should have a util function for it
    QDBusPendingCall getSubscriberCall = asyncCall("GetSubscriber");
    SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(getSubscriberCall, this);
    sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
             this, SLOT(onGetSubscriberFinished(QDBusPendingCallWatcher *)));
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

} // end namespace
