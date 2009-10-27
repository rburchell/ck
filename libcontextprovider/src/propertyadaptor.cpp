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
    sconnect((QObject*) connection->interface(),
             SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
             this, SLOT(OnServiceOwnerChanged(const QString &, const QString&, const QString&)));
    sconnect(propertyPrivate, SIGNAL(valueChanged(const QVariantList&, const quint64&)),
             this, SIGNAL(ValueChanged(const QVariantList&, const quint64&)));

    // The same value can be provided on session and system bus by
    // different providers. Unfortunately, each provider needs both a
    // system bus connection and a session bus connection to listen to
    // other providers.
    QDBusConnection::sessionBus().connect("", objectPath(), DBUS_INTERFACE, "ValueChanged",
                                                this, SLOT(onValueChanged(QVariantList, quint64)));

    QDBusConnection::systemBus().connect("", objectPath(), DBUS_INTERFACE, "ValueChanged",
                                               this, SLOT(onValueChanged(QVariantList, quint64)));
}

/// Implementation of the D-Bus method Subscribe
void PropertyAdaptor::Subscribe(const QDBusMessage &msg, QVariantList& values, quint64& timestamp)
{
    contextDebug() << "Subscribe called";

    // Store the information of the subscription. For each client, we
    // record how many times the client has subscribed.
    QString client = msg.service();

    if (clientServiceNames.contains(client) == false) {
        clientServiceNames.insert(client);
        if (clientServiceNames.size() == 1) {
            propertyPrivate->setSubscribed();
        }
    }
    else {
        contextWarning() << "Client" << client << "subscribed to property" << propertyPrivate->key << "multiple times";
        QDBusMessage error = msg.createErrorReply("org.maemo.contextkit.Error.MultipleSubscribe",
                             "Subscribing to " + propertyPrivate->key + " multiple times");
        connection->send(error);
    }

    // Construct the return values
    Get(values, timestamp);
}

/// Implementation of the D-Bus method Unsubscribe
void PropertyAdaptor::Unsubscribe(const QDBusMessage &msg)
{
    contextDebug() << "Unsubscribe called";
    QString client = msg.service();

    if (clientServiceNames.contains(client)) {
        clientServiceNames.remove(client);
        if (clientServiceNames.size() == 0) {
            propertyPrivate->setUnsubscribed();
        }
    }
    else {
        contextWarning() << "Client" << client << "unsubscribed from property" << propertyPrivate->key << "without subscribing";
        QDBusMessage error = msg.createErrorReply("org.maemo.contextkit.Error.IllegalUnsubscribe",
                             "Unsubscribing from a non-subscribed property " + propertyPrivate->key);
        connection->send(error);
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

/// Called when a NameOwnerChanged signal is broadcast on D-Bus. If
/// one of our clients has disappeared from D-Bus, update the client
/// list.
void PropertyAdaptor::OnServiceOwnerChanged(const QString &name, const QString &oldName, const QString &newName)

{
    if (newName == "") {
        if (clientServiceNames.contains(name)) {
            // It was one of our clients
            clientServiceNames.remove(name);
            if (clientServiceNames.size() == 0) {
                propertyPrivate->setUnsubscribed();
            }
        }
    }
}

/// Object path where the corresponding PropertyPrivate object should
/// be registered at. For a core propertiy Property.Name (not starting
/// with /), the path is /org/maemo/contextkit/Property/Name. For a
/// non-core property /com/my/property, the object path is
/// /com/my/property.
QString PropertyAdaptor::objectPath() const
{
    if (!propertyPrivate->key.startsWith("/"))
        return QString("/org/maemo/contextkit/") + QString(propertyPrivate->key).replace(".", "/");
    return QString(propertyPrivate->key);
}

/// Called when the service is stopped and will disappear from
/// D-Bus. If it appears again, the clients will resubscribe, and it
/// shouldn't be a MultipleSubscribe error. Thus, we need to forget
/// the clients when the service is stopped.
void PropertyAdaptor::forgetClients()
{
    clientServiceNames.clear();
    propertyPrivate->setUnsubscribed();
}

} // namespace ContextProvider
