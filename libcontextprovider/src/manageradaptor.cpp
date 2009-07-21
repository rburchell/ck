/*
 * Copyright (C) 2008, 2009 Nokia Corporation.
 *
 * Contact: Marius Vollmer <marius.vollmer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QDBusConnection>
#include "manageradaptor.h"
#include "subscriberadaptor.h"
#include "logging.h"
#include "sconnect.h"

/*!
    \class ManagerAdaptor
    \brief A DBus adaptor for implementing the org.freedesktop.ContextKit.Manager
    interface. The primary operation to perform through this adaptor is to get the
    subscriber.
*/

/// Constructor. Creates new adaptor for the given manager with the given 
/// dbus connection. The connection \a conn is not retained.
ManagerAdaptor::ManagerAdaptor(Manager* manager, QDBusConnection *conn)
    : QDBusAbstractAdaptor(manager), manager(manager), connection(conn)
{
    sconnect((QObject*) connection->interface(), SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
             this, SLOT(OnServiceOwnerChanged(const QString &, const QString&, const QString&)));
}

/// Executes Manager::getSubscriber on the managed Manager.
QDBusObjectPath ManagerAdaptor::GetSubscriber(const QDBusMessage &msg)
{
    Subscriber *subscriber = manager->getSubscriber(msg.service());

    if (!connection->objectRegisteredAt(subscriber->dbusPath())) {
        new SubscriberAdaptor(subscriber);
        connection->registerObject(subscriber->dbusPath(), subscriber);
    }
    
    return QDBusObjectPath(subscriber->dbusPath());
}

/// Dbus interface slot. The ManagerAdaptor listens for dbus bus names changing 
/// to notify the managed Manager that a bus name is gone. It does it through
/// Manager::busNameIsGone function.
void ManagerAdaptor::OnServiceOwnerChanged(const QString &name, const QString &oldName, const QString &newName)

{
    if (newName == "")
        manager->busNameIsGone(name);
}

