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

#include "propertyadaptor.h"
#include "logging.h"
#include "sconnect.h"
#include "propertyprivate.h"

#include <QDBusConnection>

namespace ContextProvider {

/*!
    \class PropertyAdaptor
    \brief A DBus adaptor for implementing the org.maemo.contextkit.Property
*/

/// Constructor. Creates new adaptor for the given manager with the given
/// dbus connection. The connection \a conn is not retained.
PropertyAdaptor::PropertyAdaptor(PropertyPrivate* propertyPrivate, QDBusConnection *conn)
    : QDBusAbstractAdaptor(propertyPrivate), propertyPrivate(propertyPrivate), connection(conn)
{
    sconnect((QObject*) connection->interface(),
             SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
             this, SLOT(OnServiceOwnerChanged(const QString &, const QString&, const QString&)));
    sconnect(propertyPrivate, SIGNAL(valueChanged(const QVariantList&, const qlonglong&)),
             this, SIGNAL(ValueChanged(const QVariantList&, const qlonglong&)));
}

void PropertyAdaptor::Subscribe(const QDBusMessage &msg, QVariantList& values, qlonglong& timestamp)
{
    contextDebug() << "Subscribe called";

    // Store the information of the subscription. For each client, we
    // record how many times the client has subscribed.
    QString client = msg.service();

    if (clientServiceNames.contains(client) == false) {
        clientServiceNames.insert(client);
        if (clientServiceNames.size() == 1) {
            emit propertyPrivate->firstSubscriberAppeared(propertyPrivate->key);
        }
    }
    else {
        contextWarning() << "Client" << client << "subscribed to property" << propertyPrivate->key << "multiple times";
        // FIXME: D-Bus error
    }

    // Construct the return values
    Get(values, timestamp);
}

void PropertyAdaptor::Unsubscribe(const QDBusMessage &msg)
{
    contextDebug() << "Unsubscribe called";
    QString client = msg.service();

    if (clientServiceNames.contains(client)) {
        clientServiceNames.remove(client);
        if (clientServiceNames.size() == 0) {
            emit propertyPrivate->lastSubscriberDisappeared(propertyPrivate->key);
        }
    }
    else {
        contextWarning() << "Client" << client << "unsubscribed from property" << propertyPrivate->key << "without subscribing";
        // FIXME: D-Bus error
    }
}

void PropertyAdaptor::Get(QVariantList& values, qlonglong& timestamp)
{
    // Construct the return values
    if (propertyPrivate->value.isNull() == false) {
        values << propertyPrivate->value;
    }
    timestamp = propertyPrivate->timestamp;
}

/// Dbus interface slot. The PropertyAdaptor listens for dbus bus names changing
/// to notify the managed Property that a bus name is gone. It does it through
/// Property::busNameIsGone function.
void PropertyAdaptor::OnServiceOwnerChanged(const QString &name, const QString &oldName, const QString &newName)

{
    if (newName == "") {
        if (clientServiceNames.contains(name)) {
            // It was one of our clients
            clientServiceNames.remove(name);
            if (clientServiceNames.size() == 0) {
                emit propertyPrivate->lastSubscriberDisappeared(propertyPrivate->key);
            }
        }
    }
}

} // namespace ContextProvider
