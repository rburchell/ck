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

void PropertyProvider::onGetSubscriberFinished(QString objectPath)
{
    qDebug() << "PropertyProvider::onGetSubscriberFinished" << objectPath;
    if (objectPath != "") {
        // GetSubscriber was successful
        getSubscriberFailed = false;

        subscriber = new SubscriberInterface(busType, busName, objectPath, this);

        sconnect(subscriber,
                 SIGNAL(valuesChanged(QMap<QString, QVariant>, bool)),
                 this,
                 SLOT(onValuesChanged(QMap<QString, QVariant>, bool)));
        sconnect(subscriber,
                 SIGNAL(subscribeFinished(QSet<QString>)),
                 this,
                 SLOT(onSubscribeFinished(QSet<QString>)));

        idleTimer.start();
    }
    else {
        // GetSubscriber failed
        getSubscriberFailed = true;
        emit subscribeFinished(toSubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
    }
}

void PropertyProvider::onSubscribeFinished(QSet<QString> keys)
{
    qDebug() << "PropertyProvider::onSubscribeFinished" << keys;
    emit subscribeFinished(keys);
}


/// Constructs a new instance. ContextProperty handles providers for
/// you, no need to (and can't) call this constructor.
PropertyProvider::PropertyProvider(QDBusConnection::BusType busType, const QString& busName)
    : busType(busType), busName(busName), managerInterface(busType, busName, this), subscriber(0),
      getSubscriberFailed(false)
{
    // Setup idle timer
    idleTimer.setSingleShot(true);
    idleTimer.setInterval(0);

    // Call GetSubscriber asynchronously
    sconnect(&managerInterface, SIGNAL(getSubscriberFinished(QString)), this, SLOT(onGetSubscriberFinished(QString)));
    sconnect(&idleTimer, SIGNAL(timeout()),
             this, SLOT(idleHandler()));
    managerInterface.getSubscriber();
}

/// Returns the dbus name and bus type of the provider
QString PropertyProvider::getName() const
{
    return ""; /// FIXME: Remove this function?
}

/// Schedules a property to be subscribed to when the main loop is
/// entered the next time.
void PropertyProvider::subscribe(PropertyHandle* handle)
{
    qDebug() << "PropertyProvider::subscribe";

    if (getSubscriberFailed) {
        qDebug() << "GetSubscriber has failed previously";
        // The subscription will never be successful
        QSet<QString> toEmit;
        toEmit.insert(handle->key());
        emit subscribeFinished(toEmit);
        return;
    }

    toSubscribe.insert(handle->key());
    toUnsubscribe.remove(handle->key());

    if (subscriber != 0)
        idleTimer.start();
}

void PropertyProvider::idleHandler()
{
    if (subscriber != 0) {
        subscriber->subscribe(toSubscribe);
        subscriber->unsubscribe(toUnsubscribe);
        toSubscribe.clear();
        toUnsubscribe.clear();
    }
}

/// Schedules a property to be unsubscribed from when the main loop is
/// entered the next time.
void PropertyProvider::unsubscribe(PropertyHandle* handle)
{
    toUnsubscribe.insert(handle->key());
    toSubscribe.remove(handle->key());

    if (subscriber != 0)
        idleTimer.start();
}

/// Slot, handling changed values coming from the provider over DBUS.
void PropertyProvider::onValuesChanged(QMap<QString, QVariant> values, bool processingSubscription)
{
    const QHash<QString, PropertyHandle*> &properties =
        PropertyManager::instance()->properties;

    // FIXME: manager should be a simple container of handles...
    for (QMap<QString, QVariant>::const_iterator i = values.constBegin(); i != values.constEnd(); ++i) {
        // Get the PropertyHandle corresponding to the property to update.
        // FIXME: handlefactory->handle(i.key())
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
