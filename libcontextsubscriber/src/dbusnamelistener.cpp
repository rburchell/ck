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

namespace ContextSubscriber {

/*! \class DBusNameListener

  \brief Extends the QDBusServiceWatcher; adds the possibility to do an
  initial check of the service status (asyncronously) and to query the status
  of the service any time.

  When you create an instance of this class, it won't open any D-Bus
  connections. When startListening is called, the instance starts using the
  QDBusServiceWatcher to get updates when the service is registered or
  unregistered.  It can also check the current status of the service by
  executing an asynchronous NameHasOwner call.

  If the specified service appears on D-Bus, it will emit the \c
  nameAppeared() signal, if disappears, then the nameDisappeared()
  signal.  An initial query and signal emission will be done if \c
  initialCheck is true, which is the default.

  Anytime you can check with <tt>isServicePresent()</tt> if according
  to our current knowledge (last signal emission) the service is
  present or not.  This means that if \c initialCheck is false,
  <tt>isServicePresent()</tt> can return false, even though the service
  is present.
*/
DBusNameListener::DBusNameListener(QDBusConnection::BusType busType, const QString &busName, QObject *parent) :
    QDBusServiceWatcher(parent),
    servicePresent(Unknown), busType(busType), busName(busName), listeningStarted(false), connection(0)
{
    // Don't do anything, until the user initiates the listening by calling
    // startListening.
    init();
}

DBusNameListener::DBusNameListener(const QDBusConnection bus, const QString &busName, QObject *parent) :
    QDBusServiceWatcher(parent),
    servicePresent(Unknown), busName(busName), listeningStarted(false), connection(0)
{
    // we copy the connection so we can safely delete it in the destructor
    connection = new QDBusConnection(bus);
    init();
}

/// Initializes the QDBusServiceWatcher base part and connects the signals
/// from it.
void DBusNameListener::init()
{
    addWatchedService(busName);
    sconnect(this, SIGNAL(serviceRegistered(const QString&)),
             this, SLOT(setServicePresent()));
    sconnect(this, SIGNAL(serviceUnregistered(const QString&)),
             this, SLOT(setServiceGone()));
}

DBusNameListener::~DBusNameListener()
{
    delete connection;
    connection = 0;
}

/// Start listening to the service registration / unregistration over
/// D-Bus. If \a nameHasOwnerCheck is true, also send a NameHasOwner query to
/// D-Bus (asyncronously).
void DBusNameListener::startListening(bool nameHasOwnerCheck)
{
    if (listeningStarted) return;

    listeningStarted = true;
    // if we were initialized without a connection, we have to create our own
    if (connection == 0) {
        if (busType == QDBusConnection::SystemBus) {
            connection = new QDBusConnection(QDBusConnection::systemBus());
        }
        else if (busType == QDBusConnection::SessionBus) {
            connection = new QDBusConnection(QDBusConnection::sessionBus());
        }
        else {
            return;
        }
    }
    // set the connection + watched service name to the QDBusServiceWatcher
    // base object; now it will start to watch
    setConnection(*connection);

    // Check if the service is already there
    if (nameHasOwnerCheck) {
        if (connection->isConnected() == false) {
            setServiceGone();
        }
        QDBusPendingCall nameHasOwnerCall = connection->interface()->asyncCall("NameHasOwner", busName);
        SafeDBusPendingCallWatcher *watcher = new SafeDBusPendingCallWatcher(nameHasOwnerCall, this);
        sconnect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
                 this, SLOT(onNameHasOwnerFinished(QDBusPendingCallWatcher *)));
    }
}

void DBusNameListener::setServicePresent()
{
    if (servicePresent != Present) {
        servicePresent = Present;
        Q_EMIT nameAppeared();
    }
}

void DBusNameListener::setServiceGone()
{
    if (servicePresent != NotPresent) {
        servicePresent = NotPresent;
        Q_EMIT nameDisappeared();
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
    else {
        // PropertyHandle's are waiting for any signal if they have initiated the
        // "is commander there" check. So notify also the opposite case.
        setServiceGone();
    }
}

/// Return our current understanding (not present, present, unknown)
/// of the presence of the watched service.
DBusNameListener::ServicePresence DBusNameListener::isServicePresent() const
{
    return servicePresent;
}

} // end namespace
