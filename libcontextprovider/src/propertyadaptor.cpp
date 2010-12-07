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

    PropertyAdaptor represents the Property object on D-Bus. It also
    keeps track of its clients and sets the PropertyPrivate to
    subscribed or unsubscribed accordingly.

    PropertyAdaptor also listens to values sent by other providers on
    D-Bus and notifies the PropertyPrivate about them.
*/

/// Constructor. Creates new adaptor for the given manager with the given
/// dbus connection. The connection \a conn is not retained.
PropertyAdaptor::PropertyAdaptor(PropertyPrivate* propertyPrivate, QDBusConnection *conn)
    : QDBusAbstractAdaptor(propertyPrivate), propertyPrivate(propertyPrivate), connection(conn)
{
    // Start listening to the QDBusServiceWathcer, to know when our client has
    // exited.
    sconnect(&serviceWatcher, SIGNAL(serviceUnregistered(const QString&)),
             this, SLOT(onClientExited(const QString&)));
    serviceWatcher.setConnection(*conn);

    sconnect(propertyPrivate, SIGNAL(valueChanged(const QVariantList&, const quint64&)),
             this, SIGNAL(ValueChanged(const QVariantList&, const quint64&)));

    // Start listening to ValueChanged signals. We only listen to the
    // same bus we're on: that means if the same property is provided
    // both on session and on system bus, overhearing won't work.
    connection->connect("", objectPath(), DBUS_INTERFACE, "ValueChanged",
                        this, SLOT(onValueChanged(QVariantList, quint64)));
}

/// Implementation of the D-Bus method Subscribe
void PropertyAdaptor::Subscribe(const QDBusMessage &msg, QVariantList& values, quint64& timestamp)
{
    contextDebug() << "Subscribe called";

    // Store the information of the subscription. For each property, we record
    // which clients have subscribed.
    QString client = msg.service();

    if (clientServiceNames.contains(client) == false) {
        clientServiceNames.insert(client);
        if (clientServiceNames.size() == 1) {
            propertyPrivate->setSubscribed();
        }
        serviceWatcher.addWatchedService(client);
    }
    else {
        contextDebug() << "Client" << client << "subscribed to property" << propertyPrivate->key << "multiple times";
    }
    // Don't treat the "client sends 2 Subscribe calls" as an error.  When the
    // provider is not running (e.g., during boot), the subscriber might send 2
    // Subscribe calls: one of them before the provider is running, and the
    // other when it notices that the provider has started.

    // Construct the return values
    Get(values, timestamp);
}

/// Implementation of the D-Bus method Unsubscribe
void PropertyAdaptor::Unsubscribe(const QDBusMessage &msg)
{
    contextDebug() << "Unsubscribe called";
    QString client = msg.service();

    if (clientServiceNames.remove(client)) {
        if (clientServiceNames.size() == 0) {
            propertyPrivate->setUnsubscribed();
        }
        serviceWatcher.removeWatchedService(client);
    }
    else {
        contextDebug() << "Client" << client << "unsubscribed from property" <<
            propertyPrivate->key << "without subscribing";
        // Don't treat the "client sends Unsubscribe for a non-subscribed
        // property" as an error.  The intention of the client is to not be
        // subscribed, so there's nothing to do.
    }
}

/// Implementation of the D-Bus method Get.
void PropertyAdaptor::Get(QVariantList& values, quint64& timestamp)
{
    // Construct the return values
    if (propertyPrivate->value.isNull() == false) {
        values << propertyPrivate->value;
    }
    timestamp = propertyPrivate->timestamp;
}

/// Called when a ValueChanged signal is overheard on D-Bus. Command
/// PropertyPrivate to update its overheard value.
void PropertyAdaptor::onValueChanged(QVariantList values, quint64 timestamp)
{
    propertyPrivate->updateOverheardValue(values, timestamp);
}

/// Called when the DBusServiceWatcher signals that one of our clients has
/// exited D-Bus.
void PropertyAdaptor::onClientExited(const QString& busName)
{
    if (clientServiceNames.remove(busName) && clientServiceNames.size() == 0) {
        propertyPrivate->setUnsubscribed();
    }
    // The client is expected to re-subscribe if it comes back. Then we will
    // start watching it again.
    serviceWatcher.removeWatchedService(busName);
}

/// Object path where the corresponding PropertyPrivate object should
/// be registered at. For a core propertiy Property.Name (not starting
/// with /), the path is /org/maemo/contextkit/Property/Name. For a
/// non-core property /com/my/property, the object path is
/// /com/my/property.
QString PropertyAdaptor::objectPath() const
{
    if (propertyPrivate->key.startsWith("/"))
        return QString(propertyPrivate->key);

    return QString("/org/maemo/contextkit/") +
            QString(propertyPrivate->key).replace(".", "/").replace(QRegExp("[^A-Za-z0-9_/]"), "_");

}

/// Called when the service is stopped and will disappear from
/// D-Bus. If it appears again, the clients will resubscribe, and it
/// shouldn't be a MultipleSubscribe error. Thus, we need to forget
/// the clients when the service is stopped.
void PropertyAdaptor::forgetClients()
{
    foreach(const QString& client, clientServiceNames)
        serviceWatcher.removeWatchedService(client);
    clientServiceNames.clear();
    propertyPrivate->setUnsubscribed();
}

} // namespace ContextProvider
