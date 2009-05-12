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

#include "propertyprovider.h"

#include "propertyhandle.h"
#include "propertymanager.h"
#include "sconnect.h"


/*!
   \class PropertyProvider

   \brief Keeps the DBUS connection with a specific provider and
   handles subscriptions with the properties of that provider.
*/

void PropertyProvider::getSubscriberFinished(QString objectPath)
{

    qDebug() << "******get2 subs finished";

    subscriber = new SubscriberInterface(busType, busName, objectPath, this);

    QStringList q;
    q << "Context.Example.Count";
    q << "Context.Example.EdgeUp";
    subscriber->subscribe(q);

    // we can keep this connected all the time
    /*sconnect(subscriber,
            SIGNAL(Changed(DBusVariantMap, const QStringList &)),
            this,
            SLOT(changeValues(const DBusVariantMap &, const QStringList &)));*/
}


/// Constructs a new instance. ContextProperty handles providers for
/// you, no need to (and can't) call this constructor.
PropertyProvider::PropertyProvider(QDBusConnection::BusType busType, const QString& busName)
    : busType(busType), busName(busName), managerInterface(busType, busName, this), subscriber(0)
{
    // Setup idle timer
    idleTimer.setSingleShot(true);
    idleTimer.setInterval(0);

    // Call GetSubscriber asynchronously
    sconnect(&managerInterface, SIGNAL(getSubscriberFinished(QString)), this, SLOT(getSubscriberFinished(QString)));
    managerInterface.getSubscriber();
}

/// Returns the dbus name and bus type of the provider
QString PropertyProvider::getName() const
{
    return ""; /// FIXME: Remove this function?
}

/// Schedules a property to be subscribed to when the main loop is
/// entered the next time.
void PropertyProvider::subscribe(PropertyHandle* prop)
{
    // include in the set
    // remove from the unsubscribe set
    // if subscriber != 0 -> reset the timer
    toSubscribe.insert(prop->key());


    QStringList unknowns;
    QStringList keys;
    keys.append(prop->key());

    if (subscriber == 0) {
        qCritical() << "This provider does not really exist!";
        return;
    }

    subscriber->subscribe(keys);

/*QDBusReply<DBusVariantMap> reply = subscriber->subscribe(keys, unknowns);
    if (!reply.isValid()) {
        qCritical() << "subscribe: bad reply from provider FIXME: " << reply.error();
        prop->value.clear(); // FIXME: Should this be changed?
        //emit prop->handleChanged();
    }

    qDebug() << "subscribed to " << prop->key << " via provider FIXME";

    if (prop->value.type() != prop->type)
        prop->value = QVariant(prop->type);
        changeValues(reply.value(), unknowns, true);*/
}

/// Schedules a property to be unsubscribed from when the main loop is
/// entered the next time.
void PropertyProvider::unsubscribe(PropertyHandle* prop)
{
    QStringList keys;
    keys.append(prop->key());

    if (subscriber == 0) {
        qCritical() << "This provider does not really exist!";
        return;
    }

    subscriber->unsubscribe(keys);

/*
    QDBusReply<void> reply = subscriber->unsubscribe(keys);

    if (!reply.isValid())
        qWarning() << "Could not unsubscribe! This should not happen!" << reply.error() <<
        " Provider: FIXME";

        qDebug() << "unsubscribed from " << prop->key << " via provider: FIXME";
*/
}

/// Slot, handling changed values coming from the provider over DBUS.
void PropertyProvider::onValuesChanged(QMap<QString, QVariant> values, bool processingSubscription)
{
    const QHash<QString, PropertyHandle*> &properties =
        PropertyManager::instance()->properties;

    // FIXME: manager should be a simple container of handles...
    for (QMap<QString, QVariant>::const_iterator i = values.constBegin();
         i != values.constEnd(); ++i) {

        // Get the PropertyHandle corresponding to the property to update.
        PropertyHandle *h = properties.value(i.key(), 0);
        if (h == 0) {
            qWarning() << "Received property not in registry:" << i.key();
            continue;
        }
        if (h->provider() != this) {
            qWarning() << "Received property not handled by this provider:" << i.key();
            continue;
        }

        // FIXME : Implement the type check here. Remember to check the types of non-nulls only.

        // Note: the null we receive is always a non-typed null. We might need to convert it here.

        QVariant newValue = i.value();
        if (h->value() == newValue && h->value().isNull() == newValue.isNull()) {
            // The property already has the value we received.
            // If we're processing the return values of Subscribe, this is normal,
            // since the return message contains values for the keys subscribed to.

            // If we're not processing the return values of Subscribe, it is an error.
            // Print out a message of provider error.
            // In either case, don't emit valueChanged.
            if (!processingSubscription) {
                qWarning() << "PROVIDER ERROR: Received unnecessary DBUS signal for property" << i.key();
            }
        }
        else {
            // The value was new
            h->setValue(newValue);
        }
    }
}

PropertyProvider::~PropertyProvider()
{
    // Note: The SubscriberInterface* subscriber was constructed with "this" as parent.
    // It doesn't need to be deleted.
}
