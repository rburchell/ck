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
#include "logging.h"

#include <QDBusConnectionInterface>
#include <QPair>
#include <QMap>
#include <QString>

/*! \class DBusNameListener

  \brief Listens for changes in a specific service name on a DBus bus,
  optionally gets the initial state of the service name.

  When you create an instance of this class, it will open a connection
  to DBus bus \c busType, and will start to listen to name changes of
  service name \c busName.  If a service appears, it will emit the \c
  nameAppeared() signal, if disappears, then the nameDisappeared()
  signal.  An initial query and signal emission will be done if \c
  initialCheck is true, which is the default.

  Anytime you can check with <tt>isServicePresent()</tt> if according
  to our current knowledge (last signal emission) the service is
  present or not.  This means that if \c initialCheck is false,
  <tt>isServicePresent()</tt> can return false, even though the service
  is present.
*/
DBusNameListener::DBusNameListener(const QDBusConnection::BusType busType, const QString &busName,
                                   bool initialCheck, QObject *parent) :
    QObject(parent), servicePresent(false), busName(busName)
{
    QDBusConnection connection("");

    // Create DBus connection
    if (busType == QDBusConnection::SessionBus) {
        connection = QDBusConnection::sessionBus();
    } else if (busType == QDBusConnection::SystemBus) {
        connection = QDBusConnection::systemBus();
    } else {
        contextCritical() << "Invalid bus type:" << busType;
        return;
    }

    if (!connection.isConnected()) {
        contextCritical() << "Couldn't connect to DBUS.";
        return;
    }

    sconnect(connection.interface(),
             SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
             this,
             SLOT(onServiceOwnerChanged(const QString&, const QString&, const QString&)));

    // Check if the service is already there
    if (initialCheck) {
        QDBusPendingCall nameHasOwnerCall = connection.interface()->asyncCall("NameHasOwner", busName);
        SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(nameHasOwnerCall, this);
        sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
                 this, SLOT(onNameHasOwnerFinished(QDBusPendingCallWatcher *)));
    }
}

/// This slot is called when DBusNameOwnerChanged signal arrives and
/// it just filters the name and if we are interested in the name it
/// emits the <tt>nameAppeared()</tt> or <tt>nameDisappeared()</tt>
/// signal.
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

/// Handling of the asynchronous reply of the initial query.
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
